#!/usr/bin/env python3
"""
Advanced LLVM IR to C Decompiler
Attempts to reconstruct C-like code from obfuscated LLVM IR
"""

import re
import sys
from typing import List, Dict, Optional

class IRToCConverter:
    """Convert LLVM IR to readable C code"""
    
    def __init__(self, ir_content: str):
        self.ir = ir_content
        self.functions: Dict[str, Dict] = {}
        self.strings: Dict[str, str] = {}
        self.globals: Dict[str, str] = {}
        
    def convert(self) -> str:
        """Convert IR to C code"""
        self._extract_strings()
        self._extract_globals()
        self._extract_functions()
        return self._generate_c_code()
    
    def _extract_strings(self):
        """Extract and decode string constants"""
        # Find string constants
        pattern = r'@(\w+)\s*=\s*.*?c"([^"]*)"'
        for match in re.finditer(pattern, self.ir):
            name = match.group(1)
            encoded = match.group(2)
            # Try to decode
            try:
                import codecs
                decoded = codecs.decode(encoded, 'unicode_escape')
                self.strings[name] = decoded
            except:
                self.strings[name] = encoded
    
    def _extract_globals(self):
        """Extract global variables"""
        pattern = r'@(\w+)\s*=\s*(?:internal\s+)?(?:private\s+)?(?:unnamed_addr\s+)?(?:global|constant)\s+([^,]+)'
        for match in re.finditer(pattern, self.ir):
            name = match.group(1)
            value = match.group(2).strip()
            self.globals[name] = value
    
    def _extract_functions(self):
        """Extract function definitions"""
        # Pattern: define return_type @function_name(params) {
        pattern = r'define\s+(?:dso_local\s+)?(?:internal\s+)?([^@]+?)\s+@(\w+)\s*\(([^)]*)\)\s*\{'
        
        for match in re.finditer(pattern, self.ir):
            return_type = match.group(1).strip()
            func_name = match.group(2)
            params = match.group(3).strip()
            
            # Skip LLVM internal functions
            if func_name.startswith('llvm.') or func_name.startswith('__'):
                continue
            
            # Extract function body
            start = match.end()
            depth = 1
            end = start
            
            while end < len(self.ir) and depth > 0:
                if self.ir[end] == '{':
                    depth += 1
                elif self.ir[end] == '}':
                    depth -= 1
                end += 1
            
            body = self.ir[start:end-1]
            
            self.functions[func_name] = {
                'return_type': return_type,
                'params': params,
                'body': body,
                'name': func_name
            }
    
    def _generate_c_code(self) -> str:
        """Generate C code from extracted information"""
        lines = []
        lines.append("// Decompiled C code from obfuscated LLVM IR")
        lines.append("// Note: This is an approximation - exact reconstruction may not be possible")
        lines.append("")
        
        # Include headers
        lines.append("#include <stdio.h>")
        lines.append("#include <stdlib.h>")
        lines.append("")
        
        # Global variables
        if self.globals:
            lines.append("// Global variables")
            for name, value in self.globals.items():
                lines.append(f"// {name} = {value}")
            lines.append("")
        
        # String constants
        if self.strings:
            lines.append("// String constants")
            for name, value in self.strings.items():
                # Escape for C
                escaped = value.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n')
                lines.append(f'// const char* {name} = "{escaped}";')
            lines.append("")
        
        # Functions
        for func_name, func_info in self.functions.items():
            lines.append(self._decompile_function(func_info))
            lines.append("")
        
        return '\n'.join(lines)
    
    def _decompile_function(self, func_info: Dict) -> str:
        """Decompile a single function"""
        lines = []
        
        # Function signature
        return_type = self._llvm_to_c_type(func_info['return_type'])
        params = self._parse_params(func_info['params'])
        param_str = ', '.join([f"{self._llvm_to_c_type(p.split()[0])} {p.split()[-1] if len(p.split()) > 1 else 'arg'}" 
                              for p in params if p.strip()])
        
        lines.append(f"{return_type} {func_info['name']}({param_str}) {{")
        
        # Decompile body
        body_lines = self._decompile_body(func_info['body'])
        for line in body_lines:
            lines.append(f"    {line}")
        
        lines.append("}")
        return '\n'.join(lines)
    
    def _decompile_body(self, body: str) -> List[str]:
        """Decompile function body"""
        lines = []
        
        # Split into basic blocks
        blocks = re.split(r'(\d+):\s*\n', body)
        
        for i, block in enumerate(blocks):
            if block.strip().isdigit():
                # Block label
                lines.append(f"// Basic block {block}:")
                continue
            
            # Process instructions in block
            instructions = block.split('\n')
            for instr in instructions:
                instr = instr.strip()
                if not instr or instr.startswith(';'):
                    continue
                
                c_instr = self._convert_instruction(instr)
                if c_instr:
                    lines.append(c_instr)
        
        return lines[:30]  # Limit to first 30 lines
    
    def _convert_instruction(self, instr: str) -> Optional[str]:
        """Convert LLVM instruction to C"""
        # Remove variable names for readability
        instr = re.sub(r'%\d+\s*=\s*', '', instr)
        
        # Return statement
        if 'ret' in instr:
            match = re.search(r'ret\s+(.+)', instr)
            if match:
                val = match.group(1).strip()
                if 'void' in val or not val or val == 'undef':
                    return "return;"
                return f"return {self._simplify_value(val)};"
        
        # Function call
        if 'call' in instr:
            match = re.search(r'call\s+[^@]*@(\w+)', instr)
            if match:
                func = match.group(1)
                if func.startswith('llvm.'):
                    return None
                # Try to find arguments
                args_match = re.search(r'\(([^)]*)\)', instr)
                if args_match:
                    args = args_match.group(1)
                    return f"{func}({self._simplify_args(args)});"
                return f"{func}();"
        
        # Arithmetic operations
        for op, symbol in [('add', '+'), ('sub', '-'), ('mul', '*'), ('udiv', '/'), ('sdiv', '/')]:
            if op in instr:
                match = re.search(f'{op}\\s+(.+?),\\s*(.+)', instr)
                if match:
                    left = self._simplify_value(match.group(1))
                    right = self._simplify_value(match.group(2))
                    return f"// {left} {symbol} {right}"
        
        # Comparisons
        for cmp, symbol in [('icmp eq', '=='), ('icmp ne', '!='), ('icmp ult', '<'), ('icmp ugt', '>')]:
            if cmp in instr:
                match = re.search(f'{re.escape(cmp)}\\s+(.+?),\\s*(.+)', instr)
                if match:
                    left = self._simplify_value(match.group(1))
                    right = self._simplify_value(match.group(2))
                    return f"// {left} {symbol} {right}"
        
        # Branch
        if 'br' in instr:
            return "// branch"
        
        # Load/Store
        if 'load' in instr or 'store' in instr:
            return "// memory operation"
        
        # Alloca (local variable)
        if 'alloca' in instr:
            return "// local variable"
        
        return None
    
    def _simplify_value(self, val: str) -> str:
        """Simplify LLVM value to readable form"""
        val = val.strip()
        
        # Integer constant
        if re.match(r'^\d+$', val):
            return val
        
        # Variable reference
        if val.startswith('%'):
            return val[1:] if len(val) > 1 else 'var'
        
        # Global reference
        if val.startswith('@'):
            return val[1:] if len(val) > 1 else 'global'
        
        return val[:20]  # Truncate long values
    
    def _simplify_args(self, args: str) -> str:
        """Simplify function arguments"""
        if not args.strip():
            return ""
        
        args_list = args.split(',')
        simplified = [self._simplify_value(arg.strip()) for arg in args_list[:5]]
        if len(args_list) > 5:
            simplified.append("...")
        return ', '.join(simplified)
    
    def _parse_params(self, params_str: str) -> List[str]:
        """Parse function parameters"""
        if not params_str.strip():
            return []
        return [p.strip() for p in params_str.split(',') if p.strip()]
    
    def _llvm_to_c_type(self, llvm_type: str) -> str:
        """Convert LLVM type to C type"""
        llvm_type = llvm_type.strip()
        
        type_map = {
            'i32': 'int',
            'i64': 'long long',
            'i8': 'char',
            'i16': 'short',
            'void': 'void',
            'float': 'float',
            'double': 'double',
            'i1': 'bool'
        }
        
        for llvm, c in type_map.items():
            if llvm in llvm_type:
                return c
        
        if 'ptr' in llvm_type or '*' in llvm_type:
            return 'void*'
        
        return 'int'

def main():
    if len(sys.argv) < 3:
        print("Usage: python ir_to_c.py <input.ll> <output.c>")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    
    print(f"Reading LLVM IR from: {input_file}")
    with open(input_file, 'r', encoding='utf-8', errors='ignore') as f:
        ir_content = f.read()
    
    print("Converting IR to C code...")
    converter = IRToCConverter(ir_content)
    c_code = converter.convert()
    
    print(f"Writing C code to: {output_file}")
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(c_code)
    
    print("✅ Conversion complete!")

if __name__ == '__main__':
    main()

