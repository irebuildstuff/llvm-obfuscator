'use client'

interface CompilationInstructionsProps {
  filename: string
  className?: string
}

export default function CompilationInstructions({ filename, className = '' }: CompilationInstructionsProps) {
  const baseFilename = filename.replace('.ll', '').replace('_obfuscated', '')

  return (
    <div className={`vt-card ${className}`}>
      <h3 className="vt-card-title mb-4 flex items-center gap-2">
        <svg className="w-5 h-5 text-vt-primary" fill="none" stroke="currentColor" viewBox="0 0 24 24">
          <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 12h6m-6 4h6m2 5H7a2 2 0 01-2-2V5a2 2 0 012-2h5.586a1 1 0 01.707.293l5.414 5.414a1 1 0 01.293.707V19a2 2 0 01-2 2z" />
        </svg>
        Compile Locally
      </h3>
      
      <p className="text-sm text-vt-text-secondary mb-4">
        For security reasons, we only provide the obfuscated LLVM IR file. 
        Compile it locally on your machine using the instructions below.
      </p>

      <div className="space-y-4">
        {/* Windows Instructions */}
        <div className="border border-vt-border rounded-lg p-4 bg-vt-bg-secondary">
          <div className="flex items-center gap-2 mb-3">
            <svg className="w-5 h-5 text-vt-primary" fill="none" stroke="currentColor" viewBox="0 0 24 24">
              <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 3v2m6-2v2M9 19v2m6-2v2M5 9H3m2 6H3m18-6h-2m2 6h-2M7 19h10a2 2 0 002-2V7a2 2 0 00-2-2H7a2 2 0 00-2 2v10a2 2 0 002 2zM9 9h6v6H9V9z" />
            </svg>
            <h4 className="font-semibold text-vt-text-primary">Windows</h4>
          </div>
          
          <div className="space-y-2 text-sm">
            <div>
              <p className="text-vt-text-tertiary mb-1">1. Install LLVM/Clang:</p>
              <p className="text-vt-text-secondary font-mono text-xs bg-vt-bg-primary p-2 rounded border border-vt-border">
                Download from: <a href="https://llvm.org/builds/" target="_blank" rel="noopener noreferrer" className="text-vt-primary hover:underline">https://llvm.org/builds/</a>
              </p>
            </div>
            
            <div>
              <p className="text-vt-text-tertiary mb-1">2. Open Command Prompt and compile:</p>
              <div className="bg-vt-bg-primary p-3 rounded border border-vt-border font-mono text-xs text-vt-text-primary overflow-x-auto">
                <div className="mb-1 text-vt-text-tertiary"># Navigate to the directory with your .ll file</div>
                <div className="mb-1">cd path\to\your\file</div>
                <div className="mb-1 text-vt-text-tertiary"># Compile the obfuscated IR</div>
                <div className="mb-1">clang "{filename}" -o "{baseFilename}.exe" -fno-exceptions</div>
                <div className="text-vt-text-tertiary"># Run the executable</div>
                <div>.\{baseFilename}.exe</div>
              </div>
            </div>
          </div>
        </div>

        {/* Linux/Mac Instructions */}
        <div className="border border-vt-border rounded-lg p-4 bg-vt-bg-secondary">
          <div className="flex items-center gap-2 mb-3">
            <svg className="w-5 h-5 text-vt-primary" fill="none" stroke="currentColor" viewBox="0 0 24 24">
              <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M10 20l4-16m4 4l4 4-4 4M6 16l-4-4 4-4" />
            </svg>
            <h4 className="font-semibold text-vt-text-primary">Linux / macOS</h4>
          </div>
          
          <div className="space-y-2 text-sm">
            <div>
              <p className="text-vt-text-tertiary mb-1">1. Install LLVM/Clang:</p>
              <div className="bg-vt-bg-primary p-2 rounded border border-vt-border font-mono text-xs text-vt-text-primary">
                <div className="mb-1"># Ubuntu/Debian:</div>
                <div className="mb-1">sudo apt-get install clang llvm</div>
                <div className="mb-1"># macOS (with Homebrew):</div>
                <div>brew install llvm</div>
              </div>
            </div>
            
            <div>
              <p className="text-vt-text-tertiary mb-1">2. Open Terminal and compile:</p>
              <div className="bg-vt-bg-primary p-3 rounded border border-vt-border font-mono text-xs text-vt-text-primary overflow-x-auto">
                <div className="mb-1 text-vt-text-tertiary"># Navigate to the directory with your .ll file</div>
                <div className="mb-1">cd /path/to/your/file</div>
                <div className="mb-1 text-vt-text-tertiary"># Compile the obfuscated IR</div>
                <div className="mb-1">clang "{filename}" -o "{baseFilename}" -fno-exceptions -lstdc++</div>
                <div className="text-vt-text-tertiary"># Run the executable</div>
                <div>./{baseFilename}</div>
              </div>
            </div>
          </div>
        </div>

        {/* Important Notes */}
        <div className="border-l-4 border-vt-primary bg-vt-primary-subtle p-4 rounded">
          <div className="flex items-start gap-2">
            <svg className="w-5 h-5 text-vt-primary flex-shrink-0 mt-0.5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
              <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M13 16h-1v-4h-1m1-4h.01M21 12a9 9 0 11-18 0 9 9 0 0118 0z" />
            </svg>
            <div className="text-sm">
              <p className="font-semibold text-vt-text-primary mb-1">Important Notes:</p>
              <ul className="text-vt-text-secondary space-y-1 list-disc list-inside">
                <li>Use <code className="bg-vt-bg-primary px-1 rounded text-xs">-fno-exceptions</code> flag to avoid exception handling issues</li>
                <li>If you encounter linking errors, try: <code className="bg-vt-bg-primary px-1 rounded text-xs">clang "{filename}" -o "{baseFilename}" -fno-exceptions -lstdc++</code></li>
                <li>For C++ code, ensure you have the C++ standard library installed</li>
                <li>The obfuscated code maintains the same functionality as the original</li>
              </ul>
            </div>
          </div>
        </div>
      </div>
    </div>
  )
}

