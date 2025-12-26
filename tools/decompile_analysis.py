#!/usr/bin/env python3
"""
LLVM IR to C Decompilation and Analysis Tool
Converts obfuscated LLVM IR back to readable C-like code and generates comparison reports
"""

import re
import sys
import os
from pathlib import Path
from typing import Dict, List, Tuple, Optional
from dataclasses import dataclass
from collections import defaultdict

@dataclass
class FunctionInfo:
    name: str
    params: List[str]
    return_type: str
    instructions: List[str]
    basic_blocks: int
    complexity: int

@dataclass
class ObfuscationMetrics:
    total_functions: int
    obfuscated_functions: int
    string_encryptions: int
    opaque_predicates: int
    bogus_code_blocks: int
    fake_loops: int
    virtualized_functions: int
    control_flow_obfuscations: int
    total_instructions: int
    obfuscated_instructions: int

class LLVMIRParser:
    """Parse LLVM IR and extract readable information"""
    
    def __init__(self, ir_file: str):
        self.ir_file = ir_file
        self.content = self._read_file()
        self.functions: Dict[str, FunctionInfo] = {}
        self.globals: List[str] = []
        self.strings: List[str] = []
        self.obfuscation_patterns = defaultdict(int)
        
    def _read_file(self) -> str:
        """Read LLVM IR file"""
        with open(self.ir_file, 'r', encoding='utf-8', errors='ignore') as f:
            return f.read()
    
    def parse(self):
        """Parse the IR file"""
        self._extract_globals()
        self._extract_strings()
        self._extract_functions()
        self._detect_obfuscation_patterns()
    
    def _extract_globals(self):
        """Extract global variables"""
        pattern = r'@(\w+)\s*=\s*(?:internal\s+)?(?:private\s+)?(?:unnamed_addr\s+)?(?:global|constant)'
        self.globals = re.findall(pattern, self.content)
    
    def _extract_strings(self):
        """Extract string literals"""
        # Pattern for string constants
        pattern = r'c"([^"]*)"'
        matches = re.findall(pattern, self.content)
        self.strings = [self._decode_string(m) for m in matches if m]
    
    def _decode_string(self, s: str) -> str:
        """Decode escaped string"""
        try:
            # Use codecs to properly decode escape sequences
            import codecs
            return codecs.decode(s, 'unicode_escape')
        except:
            return s
    
    def _extract_functions(self):
        """Extract function definitions"""
        # Pattern for function definitions
        func_pattern = r'define\s+(?:dso_local\s+)?(?:internal\s+)?([^@]+?)\s+@(\w+)\s*\(([^)]*)\)\s*\{'
        
        for match in re.finditer(func_pattern, self.content):
            return_type = match.group(1).strip()
            func_name = match.group(2)
            params = match.group(3).strip()
            
            # Find function body
            start_pos = match.end()
            brace_count = 1
            end_pos = start_pos
            
            while end_pos < len(self.content) and brace_count > 0:
                if self.content[end_pos] == '{':
                    brace_count += 1
                elif self.content[end_pos] == '}':
                    brace_count -= 1
                end_pos += 1
            
            func_body = self.content[start_pos:end_pos-1]
            
            # Count basic blocks
            basic_blocks = len(re.findall(r'^\s*\d+:', func_body, re.MULTILINE))
            
            # Count instructions
            instructions = re.findall(r'^\s*[^;]+', func_body, re.MULTILINE)
            instructions = [i.strip() for i in instructions if i.strip() and not i.strip().startswith(';')]
            
            # Calculate complexity (rough estimate)
            complexity = len(instructions) + basic_blocks * 2
            
            self.functions[func_name] = FunctionInfo(
                name=func_name,
                params=self._parse_params(params),
                return_type=return_type,
                instructions=instructions,
                basic_blocks=basic_blocks,
                complexity=complexity
            )
    
    def _parse_params(self, params_str: str) -> List[str]:
        """Parse function parameters"""
        if not params_str:
            return []
        params = []
        for param in params_str.split(','):
            param = param.strip()
            if param:
                # Extract parameter name if present
                match = re.search(r'%\w+', param)
                if match:
                    params.append(match.group())
                else:
                    params.append(param)
        return params
    
    def _detect_obfuscation_patterns(self):
        """Detect obfuscation patterns in IR"""
        # String encryption patterns
        if re.search(r'@__obf_key', self.content):
            self.obfuscation_patterns['string_encryption'] = len(re.findall(r'@__obf_key', self.content))
        
        # Opaque predicates
        if re.search(r'opaque|obfuscated', self.content, re.IGNORECASE):
            self.obfuscation_patterns['opaque_predicates'] = len(re.findall(r'opaque|obfuscated', self.content, re.IGNORECASE))
        
        # Bogus code (dead code patterns)
        if re.search(r'0xDEADBEEF|0xCAFEBABE', self.content):
            self.obfuscation_patterns['bogus_code'] = len(re.findall(r'0xDEADBEEF|0xCAFEBABE', self.content))
        
        # Fake loops
        if re.search(r'fake|dummy.*loop', self.content, re.IGNORECASE):
            self.obfuscation_patterns['fake_loops'] = len(re.findall(r'fake|dummy.*loop', self.content, re.IGNORECASE))
        
        # Virtualization
        if re.search(r'@__obf_decrypt|@__vm_|bytecode', self.content, re.IGNORECASE):
            self.obfuscation_patterns['virtualization'] = len(re.findall(r'@__obf_decrypt|@__vm_|bytecode', self.content, re.IGNORECASE))
        
        # Control flow obfuscation
        if re.search(r'switch.*dispatch|flatten', self.content, re.IGNORECASE):
            self.obfuscation_patterns['control_flow'] = len(re.findall(r'switch.*dispatch|flatten', self.content, re.IGNORECASE))
    
    def get_metrics(self) -> ObfuscationMetrics:
        """Calculate obfuscation metrics"""
        total_instructions = sum(len(f.instructions) for f in self.functions.values())
        
        return ObfuscationMetrics(
            total_functions=len(self.functions),
            obfuscated_functions=len([f for f in self.functions.values() if f.complexity > 10]),
            string_encryptions=self.obfuscation_patterns.get('string_encryption', 0),
            opaque_predicates=self.obfuscation_patterns.get('opaque_predicates', 0),
            bogus_code_blocks=self.obfuscation_patterns.get('bogus_code', 0),
            fake_loops=self.obfuscation_patterns.get('fake_loops', 0),
            virtualized_functions=self.obfuscation_patterns.get('virtualization', 0),
            control_flow_obfuscations=self.obfuscation_patterns.get('control_flow', 0),
            total_instructions=total_instructions,
            obfuscated_instructions=total_instructions
        )

class CDecompiler:
    """Convert LLVM IR to C-like code"""
    
    def __init__(self, parser: LLVMIRParser):
        self.parser = parser
    
    def decompile_function(self, func_info: FunctionInfo) -> str:
        """Decompile a function to C-like code"""
        lines = []
        
        # Function signature
        return_type = self._convert_type(func_info.return_type)
        params = ', '.join([f"int {p}" if not p.startswith('%') else f"int {p[1:]}" for p in func_info.params])
        
        lines.append(f"{return_type} {func_info.name}({params}) {{")
        
        # Decompile instructions (simplified)
        for instr in func_info.instructions[:20]:  # Limit to first 20 for readability
            c_instr = self._convert_instruction(instr)
            if c_instr:
                lines.append(f"    {c_instr}")
        
        if len(func_info.instructions) > 20:
            lines.append(f"    // ... {len(func_info.instructions) - 20} more instructions ...")
        
        lines.append("}")
        return '\n'.join(lines)
    
    def _convert_type(self, llvm_type: str) -> str:
        """Convert LLVM type to C type"""
        type_map = {
            'i32': 'int',
            'i64': 'long',
            'i8': 'char',
            'void': 'void',
            'float': 'float',
            'double': 'double'
        }
        
        for llvm, c in type_map.items():
            if llvm in llvm_type:
                return c
        
        if 'ptr' in llvm_type or '*' in llvm_type:
            return 'void*'
        
        return 'int'
    
    def _convert_instruction(self, instr: str) -> Optional[str]:
        """Convert LLVM instruction to C-like code"""
        # Remove variable assignments for readability
        instr = re.sub(r'%\d+\s*=\s*', '', instr)
        
        # Convert common patterns
        if 'call' in instr:
            # Function call
            match = re.search(r'call\s+[^@]*@(\w+)', instr)
            if match:
                func_name = match.group(1)
                if func_name.startswith('llvm.'):
                    return None  # Skip LLVM intrinsics
                return f"{func_name}(...);"
        
        if 'ret' in instr:
            match = re.search(r'ret\s+(.+)', instr)
            if match:
                val = match.group(1).strip()
                if val == 'void' or not val:
                    return "return;"
                return f"return {val};"
        
        if 'add' in instr or 'sub' in instr or 'mul' in instr:
            match = re.search(r'(add|sub|mul)\s+(.+?),\s*(.+)', instr)
            if match:
                op = {'add': '+', 'sub': '-', 'mul': '*'}[match.group(1)]
                return f"// {match.group(2)} {op} {match.group(3)}"
        
        if 'br' in instr:
            return "// branch instruction"
        
        if 'load' in instr or 'store' in instr:
            return "// memory operation"
        
        return None

class ReportGenerator:
    """Generate comparison reports"""
    
    def __init__(self, original_c: str, obfuscated_ir: str, parser: LLVMIRParser):
        self.original_c = original_c
        self.obfuscated_ir = obfuscated_ir
        self.parser = parser
        self.decompiler = CDecompiler(parser)
    
    def generate_report(self, output_file: str):
        """Generate HTML report"""
        metrics = self.parser.get_metrics()
        
        html = f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Obfuscation Analysis Report</title>
    <style>
        body {{
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            margin: 0;
            padding: 20px;
            background: #f5f5f5;
        }}
        .container {{
            max-width: 1400px;
            margin: 0 auto;
            background: white;
            padding: 30px;
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }}
        h1 {{
            color: #2c3e50;
            border-bottom: 3px solid #3498db;
            padding-bottom: 10px;
        }}
        h2 {{
            color: #34495e;
            margin-top: 30px;
        }}
        .comparison {{
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 20px;
            margin: 20px 0;
        }}
        .code-block {{
            background: #2d2d2d;
            color: #f8f8f2;
            padding: 15px;
            border-radius: 5px;
            overflow-x: auto;
            font-family: 'Consolas', 'Monaco', monospace;
            font-size: 14px;
            line-height: 1.5;
        }}
        .original {{
            border-left: 4px solid #27ae60;
        }}
        .obfuscated {{
            border-left: 4px solid #e74c3c;
        }}
        .metrics {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 15px;
            margin: 20px 0;
        }}
        .metric-card {{
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 20px;
            border-radius: 8px;
            text-align: center;
        }}
        .metric-value {{
            font-size: 32px;
            font-weight: bold;
            margin: 10px 0;
        }}
        .metric-label {{
            font-size: 14px;
            opacity: 0.9;
        }}
        .obfuscation-techniques {{
            background: #ecf0f1;
            padding: 20px;
            border-radius: 5px;
            margin: 20px 0;
        }}
        .technique {{
            padding: 10px;
            margin: 5px 0;
            background: white;
            border-left: 3px solid #3498db;
            border-radius: 3px;
        }}
        .highlight {{
            background: #fff3cd;
            padding: 2px 4px;
            border-radius: 3px;
        }}
        pre {{
            margin: 0;
            white-space: pre-wrap;
            word-wrap: break-word;
        }}
        .summary {{
            background: #e8f5e9;
            padding: 20px;
            border-radius: 5px;
            border-left: 4px solid #4caf50;
            margin: 20px 0;
        }}
    </style>
</head>
<body>
    <div class="container">
        <h1>🔒 LLVM Obfuscation Analysis Report</h1>
        
        <div class="summary">
            <h2>Executive Summary</h2>
            <p>This report compares the original C source code with the obfuscated LLVM IR output, 
            demonstrating the effectiveness of the obfuscation techniques applied.</p>
        </div>
        
        <h2>📊 Obfuscation Metrics</h2>
        <div class="metrics">
            <div class="metric-card">
                <div class="metric-label">Total Functions</div>
                <div class="metric-value">{metrics.total_functions}</div>
            </div>
            <div class="metric-card">
                <div class="metric-label">Obfuscated Functions</div>
                <div class="metric-value">{metrics.obfuscated_functions}</div>
            </div>
            <div class="metric-card">
                <div class="metric-label">String Encryptions</div>
                <div class="metric-value">{metrics.string_encryptions}</div>
            </div>
            <div class="metric-card">
                <div class="metric-label">Opaque Predicates</div>
                <div class="metric-value">{metrics.opaque_predicates}</div>
            </div>
            <div class="metric-card">
                <div class="metric-label">Bogus Code Blocks</div>
                <div class="metric-value">{metrics.bogus_code_blocks}</div>
            </div>
            <div class="metric-card">
                <div class="metric-label">Fake Loops</div>
                <div class="metric-value">{metrics.fake_loops}</div>
            </div>
            <div class="metric-card">
                <div class="metric-label">Virtualized Functions</div>
                <div class="metric-value">{metrics.virtualized_functions}</div>
            </div>
            <div class="metric-card">
                <div class="metric-label">Total Instructions</div>
                <div class="metric-value">{metrics.total_instructions}</div>
            </div>
        </div>
        
        <h2>🔍 Code Comparison</h2>
        <div class="comparison">
            <div>
                <h3>✅ Original C Source Code</h3>
                <div class="code-block original">
                    <pre>{self._escape_html(self.original_c)}</pre>
                </div>
            </div>
            <div>
                <h3>🔐 Obfuscated LLVM IR (Decompiled)</h3>
                <div class="code-block obfuscated">
                    <pre>{self._generate_decompiled_code()}</pre>
                </div>
            </div>
        </div>
        
        <h2>🛡️ Applied Obfuscation Techniques</h2>
        <div class="obfuscation-techniques">
            {self._generate_techniques_list()}
        </div>
        
        <h2>📈 Complexity Analysis</h2>
        <div class="code-block">
            <pre>{self._generate_complexity_analysis()}</pre>
        </div>
        
        <h2>💡 Key Observations</h2>
        <div class="summary">
            <ul>
                <li><strong>Code Size:</strong> The obfuscated code contains <span class="highlight">{metrics.total_instructions}</span> instructions compared to the original simple program</li>
                <li><strong>Readability:</strong> The obfuscated code is significantly harder to understand without specialized tools</li>
                <li><strong>Protection Level:</strong> Multiple layers of obfuscation make reverse engineering extremely difficult</li>
                <li><strong>Performance Impact:</strong> Obfuscation adds runtime overhead but maintains functionality</li>
            </ul>
        </div>
    </div>
</body>
</html>"""
        
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write(html)
        
        print(f"✅ Report generated: {output_file}")
    
    def _escape_html(self, text: str) -> str:
        """Escape HTML special characters"""
        return (text.replace('&', '&amp;')
                   .replace('<', '&lt;')
                   .replace('>', '&gt;')
                   .replace('"', '&quot;')
                   .replace("'", '&#39;'))
    
    def _generate_decompiled_code(self) -> str:
        """Generate decompiled C-like code from IR"""
        lines = []
        for func_name, func_info in self.parser.functions.items():
            if not func_name.startswith('__'):
                lines.append(self.decompiler.decompile_function(func_info))
                lines.append("")
        
        if not lines:
            # Fallback: show key parts of IR
            ir_lines = self.obfuscated_ir.split('\n')[:50]
            return '\n'.join(ir_lines) + "\n\n// ... (truncated for display) ..."
        
        return '\n'.join(lines)
    
    def _generate_techniques_list(self) -> str:
        """Generate list of detected obfuscation techniques"""
        techniques = []
        patterns = self.parser.obfuscation_patterns
        
        if patterns.get('string_encryption', 0) > 0:
            techniques.append('<div class="technique"><strong>String Encryption:</strong> Strings are encrypted at compile time and decrypted at runtime</div>')
        
        if patterns.get('opaque_predicates', 0) > 0:
            techniques.append('<div class="technique"><strong>Opaque Predicates:</strong> Conditional branches with always-true/false conditions to confuse analysis</div>')
        
        if patterns.get('bogus_code', 0) > 0:
            techniques.append('<div class="technique"><strong>Bogus Code:</strong> Dead code blocks inserted to mislead reverse engineers</div>')
        
        if patterns.get('fake_loops', 0) > 0:
            techniques.append('<div class="technique"><strong>Fake Loops:</strong> Loops that never execute to add complexity</div>')
        
        if patterns.get('virtualization', 0) > 0:
            techniques.append('<div class="technique"><strong>Code Virtualization:</strong> Functions converted to bytecode interpreted by a virtual machine</div>')
        
        if patterns.get('control_flow', 0) > 0:
            techniques.append('<div class="technique"><strong>Control Flow Flattening:</strong> Control flow restructured using switch-based dispatchers</div>')
        
        if not techniques:
            techniques.append('<div class="technique">Basic obfuscation techniques applied (instruction substitution, variable renaming)</div>')
        
        return '\n'.join(techniques)
    
    def _generate_complexity_analysis(self) -> str:
        """Generate complexity analysis"""
        lines = []
        lines.append("Function Complexity Analysis:")
        lines.append("=" * 50)
        
        for func_name, func_info in self.parser.functions.items():
            if not func_name.startswith('__'):
                lines.append(f"\nFunction: {func_name}")
                lines.append(f"  - Basic Blocks: {func_info.basic_blocks}")
                lines.append(f"  - Instructions: {len(func_info.instructions)}")
                lines.append(f"  - Complexity Score: {func_info.complexity}")
        
        return '\n'.join(lines)

def main():
    """Main entry point"""
    if len(sys.argv) < 4:
        print("Usage: python decompile_analysis.py <original.c> <obfuscated.ll> <output.html>")
        sys.exit(1)
    
    original_c_file = sys.argv[1]
    obfuscated_ir_file = sys.argv[2]
    output_file = sys.argv[3]
    
    # Read original C code
    with open(original_c_file, 'r', encoding='utf-8') as f:
        original_c = f.read()
    
    # Parse obfuscated IR
    print(f"📖 Parsing obfuscated IR: {obfuscated_ir_file}")
    parser = LLVMIRParser(obfuscated_ir_file)
    parser.parse()
    
    # Read obfuscated IR for display
    with open(obfuscated_ir_file, 'r', encoding='utf-8', errors='ignore') as f:
        obfuscated_ir = f.read()
    
    # Generate report
    print(f"📊 Generating analysis report...")
    generator = ReportGenerator(original_c, obfuscated_ir, parser)
    generator.generate_report(output_file)
    
    print(f"\n✅ Analysis complete!")
    print(f"   Original C: {original_c_file}")
    print(f"   Obfuscated IR: {obfuscated_ir_file}")
    print(f"   Report: {output_file}")

if __name__ == '__main__':
    main()

