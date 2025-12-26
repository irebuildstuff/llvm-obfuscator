'use client'

import Link from 'next/link'

const obfuscationTechniques = [
  {
    name: 'Control Flow Obfuscation',
    description: 'Inserts opaque predicates and complex control flow structures to confuse static analysis',
    category: 'Basic',
    available: true
  },
  {
    name: 'String Encryption',
    description: 'Encrypts string literals using RC4/PBKDF2 encryption with lazy decryption',
    category: 'Basic',
    available: true
  },
  {
    name: 'Bogus Code Insertion',
    description: 'Inserts meaningless instructions that never execute but confuse analysis',
    category: 'Basic',
    available: true
  },
  {
    name: 'Fake Loop Insertion',
    description: 'Creates dead loops with false conditions to increase complexity',
    category: 'Basic',
    available: true
  },
  {
    name: 'Instruction Substitution',
    description: 'Replaces simple operations with complex equivalent expressions',
    category: 'Advanced',
    available: true
  },
  {
    name: 'Control Flow Flattening',
    description: 'Transforms natural control flow into switch-based dispatcher',
    category: 'Advanced',
    available: true
  },
  {
    name: 'Mixed Boolean Arithmetic (MBA)',
    description: 'Replaces arithmetic operations with boolean algebra equivalents',
    category: 'Advanced',
    available: true
  },
  {
    name: 'Indirect Function Calls',
    description: 'Replaces direct function calls with indirect calls through function pointers',
    category: 'Advanced',
    available: true
  },
  {
    name: 'Constant Obfuscation',
    description: 'Obfuscates numeric constants using complex expressions',
    category: 'Advanced',
    available: true
  },
  {
    name: 'Anti-Debugging',
    description: 'Detects and responds to debugger presence using RDTSC, hardware breakpoints, and PEB checks',
    category: 'Security',
    available: true
  },
  {
    name: 'Anti-Tampering',
    description: 'Adds integrity checks to detect code modification',
    category: 'Security',
    available: true
  },
  {
    name: 'Code Virtualization',
    description: 'Transforms native instructions into bytecode for custom VM',
    category: 'Elite',
    available: true
  },
  {
    name: 'Polymorphic Code',
    description: 'Creates multiple variants of the same function with runtime dispatcher',
    category: 'Elite',
    available: true
  },
  {
    name: 'Anti-Analysis',
    description: 'Detects common analysis tools and responds accordingly',
    category: 'Security',
    available: true
  },
  {
    name: 'Metamorphic Transforms',
    description: 'Applies code transformations that change on each execution',
    category: 'Elite',
    available: true
  },
  {
    name: 'Dynamic Obfuscation',
    description: 'Applies runtime obfuscation techniques',
    category: 'Elite',
    available: true
  }
]

const presets = [
  {
    name: 'Light',
    description: 'Basic protection with minimal overhead (5-10%)',
    techniques: ['Control Flow Obfuscation', 'String Encryption'],
    cycles: 1,
    bogusPercent: 10
  },
  {
    name: 'Medium',
    description: 'Balanced security and performance (15-25%)',
    techniques: ['Control Flow Obfuscation', 'String Encryption', 'Bogus Code Insertion'],
    cycles: 3,
    bogusPercent: 30
  },
  {
    name: 'Heavy',
    description: 'Maximum protection with higher overhead (50-200%)',
    techniques: [
      'Control Flow Obfuscation',
      'String Encryption',
      'Bogus Code Insertion',
      'Instruction Substitution',
      'Control Flow Flattening',
      'Mixed Boolean Arithmetic (MBA)',
      'Anti-Debugging'
    ],
    cycles: 5,
    bogusPercent: 50
  },
  {
    name: 'Custom',
    description: 'Full control over all obfuscation options',
    techniques: ['All techniques available'],
    cycles: 'Configurable',
    bogusPercent: 'Configurable'
  }
]

export default function PresetsPage() {
  return (
    <main className="min-h-screen bg-vt-bg-primary">
      {/* Header */}
      <header className="border-b border-vt-border bg-vt-bg-secondary">
        <div className="vt-container py-4">
          <div className="flex items-center justify-between">
            <Link href="/" className="text-2xl font-semibold hover:text-vt-primary transition-colors">
              LLVM Code Obfuscator
            </Link>
            <nav className="flex items-center gap-4">
              <Link href="/" className="vt-link">Home</Link>
            </nav>
          </div>
        </div>
      </header>

      <div className="vt-container py-4 sm:py-8">
        <div className="mb-6 sm:mb-8">
          <h1 className="text-2xl sm:text-4xl font-bold mb-3 sm:mb-4">Obfuscation Presets</h1>
          <p className="text-vt-text-tertiary text-base sm:text-lg">
            Choose from pre-configured presets or customize your obfuscation settings
          </p>
        </div>

        {/* Presets Grid */}
        <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-4 sm:gap-6 mb-8 sm:mb-12">
          {presets.map((preset) => (
            <div 
              key={preset.name} 
              className="vt-card hover-lift border-2 border-vt-border hover:border-vt-primary/50 transition-all duration-300 cursor-pointer group relative overflow-hidden"
            >
              {/* Gradient overlay on hover */}
              <div className="absolute inset-0 bg-gradient-to-br from-vt-primary/5 via-transparent to-vt-primary/10 opacity-0 group-hover:opacity-100 transition-opacity duration-300"></div>
              
              {/* Content */}
              <div className="relative z-10">
                <div className="flex items-center justify-between mb-3">
                  <h3 className="text-xl font-semibold text-vt-text-primary group-hover:text-vt-primary transition-colors duration-300">
                    {preset.name}
                  </h3>
                  <div className="w-2 h-2 bg-vt-primary rounded-full opacity-0 group-hover:opacity-100 transition-opacity duration-300"></div>
                </div>
                <p className="text-sm text-vt-text-tertiary mb-4 group-hover:text-vt-text-secondary transition-colors duration-300">
                  {preset.description}
                </p>
                <div className="space-y-2 mb-4">
                  <div className="flex items-center justify-between">
                    <span className="text-xs text-vt-text-tertiary">Obfuscation Cycles:</span>
                    <span className="text-sm font-semibold text-vt-text-primary">{preset.cycles}</span>
                  </div>
                  <div className="flex items-center justify-between">
                    <span className="text-xs text-vt-text-tertiary">Bogus Code %:</span>
                    <span className="text-sm font-semibold text-vt-text-primary">{preset.bogusPercent}</span>
                  </div>
                </div>
                <div className="border-t border-vt-border pt-4 group-hover:border-vt-primary/30 transition-colors duration-300">
                  <p className="text-xs font-semibold mb-2 text-vt-text-secondary group-hover:text-vt-primary transition-colors duration-300">
                    Techniques:
                  </p>
                  <ul className="text-xs text-vt-text-tertiary space-y-1.5">
                    {preset.techniques.slice(0, 3).map((tech, idx) => (
                      <li key={idx} className="flex items-center gap-2 group-hover:text-vt-text-secondary transition-colors duration-300">
                        <span className="w-1.5 h-1.5 bg-vt-primary rounded-full opacity-0 group-hover:opacity-100 transition-opacity duration-300"></span>
                        <span>{tech}</span>
                      </li>
                    ))}
                    {preset.techniques.length > 3 && (
                      <li className="text-vt-primary font-medium mt-2 group-hover:text-vt-primary-hover transition-colors duration-300">
                        + {preset.techniques.length - 3} more
                      </li>
                    )}
                  </ul>
                </div>
              </div>
            </div>
          ))}
        </div>

        {/* All Techniques */}
        <div className="mb-6 sm:mb-8">
          <h2 className="text-2xl sm:text-3xl font-bold mb-4 sm:mb-6">All Obfuscation Techniques</h2>
          
          <div className="space-y-6 sm:space-y-8">
            {['Basic', 'Advanced', 'Security', 'Elite'].map((category) => (
              <div key={category}>
                <h3 className="text-lg sm:text-xl font-semibold mb-3 sm:mb-4 text-vt-primary">{category} Techniques</h3>
                <div className="grid grid-cols-1 sm:grid-cols-2 gap-3 sm:gap-4">
                  {obfuscationTechniques
                    .filter(tech => tech.category === category)
                    .map((technique) => (
                      <div key={technique.name} className="vt-card hover-lift transition-all duration-200">
                        <h4 className="font-semibold text-vt-text-primary mb-2">{technique.name}</h4>
                        <p className="text-sm text-vt-text-tertiary">{technique.description}</p>
                      </div>
                    ))}
                </div>
              </div>
            ))}
          </div>
        </div>

        <div className="text-center mt-6 sm:mt-8">
          <Link href="/" className="vt-btn px-6 sm:px-8 py-2.5 sm:py-3 w-full sm:w-auto inline-block">
            Start Obfuscating
          </Link>
        </div>
      </div>
    </main>
  )
}

