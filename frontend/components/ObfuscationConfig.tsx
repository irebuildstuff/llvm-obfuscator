'use client'

import { useState, useEffect, useMemo } from 'react'
import SecurityMeter from './SecurityMeter'
import StatCard from './StatCard'
import ScrollReveal from './ScrollReveal'

interface ObfuscationConfigProps {
  config: {
    preset: 'basic' | 'light' | 'medium' | 'heavy' | 'custom'
    options: Record<string, any>
  }
  onChange: (config: ObfuscationConfigProps['config']) => void
}

const presetDefaults = {
  basic: {
    enableControlFlowObfuscation: true,
    enableStringEncryption: false,
    enableBogusCode: false,
    enableFakeLoops: false,
    enableInstructionSubstitution: false,
    enableControlFlowFlattening: false,
    enableMBA: false,
    enableAntiDebug: false,
    enableIndirectCalls: false,
    enableAntiTamper: false,
    enablePolymorphic: false,
    enableMetamorphic: false,
    stringEncryptionMethod: 'XOR_ROTATING',
    pbkdf2Iterations: 500,
    decryptStringsAtStartup: false,
    obfuscationCycles: 1,
    bogusCodePercentage: 10,
    fakeLoopCount: 3,
    mbaComplexity: 2,
    polymorphicVariants: 3
  },
  light: {
    enableControlFlowObfuscation: true,
    enableStringEncryption: true,
    enableBogusCode: false,
    enableFakeLoops: false,
    enableInstructionSubstitution: false,
    enableControlFlowFlattening: false,
    enableMBA: false,
    enableAntiDebug: false,
    enableIndirectCalls: false,
    enableAntiTamper: false,
    enablePolymorphic: false,
    enableMetamorphic: false,
    stringEncryptionMethod: 'RC4_SIMPLE',
    pbkdf2Iterations: 500,
    decryptStringsAtStartup: false,
    obfuscationCycles: 1,
    bogusCodePercentage: 10,
    fakeLoopCount: 3,
    mbaComplexity: 2,
    polymorphicVariants: 3
  },
  medium: {
    enableControlFlowObfuscation: true,
    enableStringEncryption: true,
    enableBogusCode: true,
    enableFakeLoops: true,
    enableInstructionSubstitution: false,
    enableControlFlowFlattening: false,
    enableMBA: false,
    enableAntiDebug: true,
    enableIndirectCalls: true,
    enableAntiTamper: false,
    enablePolymorphic: false,
    enableMetamorphic: false,
    stringEncryptionMethod: 'RC4_PBKDF2',
    pbkdf2Iterations: 1000,
    decryptStringsAtStartup: false,
    obfuscationCycles: 3,
    bogusCodePercentage: 30,
    fakeLoopCount: 5,
    mbaComplexity: 3,
    polymorphicVariants: 5
  },
  heavy: {
    enableControlFlowObfuscation: true,
    enableStringEncryption: true,
    enableBogusCode: true,
    enableFakeLoops: true,
    enableInstructionSubstitution: true,
    enableControlFlowFlattening: true,
    enableMBA: true,
    enableAntiDebug: true,
    enableIndirectCalls: true,
    enableAntiTamper: true,
    enablePolymorphic: true,
    enableMetamorphic: true,
    stringEncryptionMethod: 'RC4_PBKDF2',
    pbkdf2Iterations: 2000,
    decryptStringsAtStartup: false,
    obfuscationCycles: 5,
    bogusCodePercentage: 50,
    fakeLoopCount: 8,
    mbaComplexity: 5,
    polymorphicVariants: 5
  },
  custom: {
    enableControlFlowObfuscation: true,
    enableStringEncryption: true,
    enableBogusCode: false,
    enableFakeLoops: false,
    enableInstructionSubstitution: false,
    enableControlFlowFlattening: false,
    enableMBA: false,
    enableAntiDebug: false,
    enableIndirectCalls: false,
    enableAntiTamper: false,
    enablePolymorphic: false,
    enableMetamorphic: false,
    stringEncryptionMethod: 'RC4_PBKDF2',
    pbkdf2Iterations: 1000,
    decryptStringsAtStartup: false,
    obfuscationCycles: 3,
    bogusCodePercentage: 30,
    fakeLoopCount: 5,
    mbaComplexity: 3,
    polymorphicVariants: 3
  }
}

const presetInfo = {
  basic: {
    name: 'Basic',
    description: 'Minimal protection, fastest execution',
    overhead: '5-10%',
    security: 'Low'
  },
  light: {
    name: 'Light',
    description: 'Basic protection with string encryption',
    overhead: '10-15%',
    security: 'Medium'
  },
  medium: {
    name: 'Medium',
    description: 'Balanced security with advanced features',
    overhead: '20-40%',
    security: 'High'
  },
  heavy: {
    name: 'Heavy',
    description: 'World-class maximum protection',
    overhead: '50-200%',
    security: 'Maximum'
  },
  custom: {
    name: 'Custom',
    description: 'Full control over all options',
    overhead: 'Variable',
    security: 'Variable'
  }
}

export default function ObfuscationConfig({ config, onChange }: ObfuscationConfigProps) {
  const [showAdvanced, setShowAdvanced] = useState(false)
  const [activeTab, setActiveTab] = useState<'core' | 'advanced' | 'string' | 'params'>('core')

  // Calculate security level and strength
  const securityMetrics = useMemo(() => {
    const options = config.options
    let strength = 0
    let enabledFeatures = 0
    const totalFeatures = 15

    // Core features
    if (options.enableControlFlowObfuscation) { strength += 10; enabledFeatures++ }
    if (options.enableStringEncryption) { 
      strength += options.stringEncryptionMethod === 'RC4_PBKDF2' ? 15 : 8
      enabledFeatures++
    }
    if (options.enableBogusCode) { strength += 8; enabledFeatures++ }
    if (options.enableFakeLoops) { strength += 5; enabledFeatures++ }
    if (options.enableInstructionSubstitution) { strength += 7; enabledFeatures++ }
    if (options.enableControlFlowFlattening) { strength += 12; enabledFeatures++ }
    if (options.enableMBA) { strength += 10; enabledFeatures++ }

    // Advanced features
    if (options.enableAntiDebug) { strength += 15; enabledFeatures++ }
    if (options.enableIndirectCalls) { strength += 8; enabledFeatures++ }
    if (options.enableAntiTamper) { strength += 12; enabledFeatures++ }
    if (options.enablePolymorphic) { strength += 10; enabledFeatures++ }
    if (options.enableMetamorphic) { strength += 10; enabledFeatures++ }

    // Cycles multiplier
    const cycles = options.obfuscationCycles || 1
    strength *= (1 + (cycles - 1) * 0.2)

    // PBKDF2 iterations bonus
    if (options.stringEncryptionMethod === 'RC4_PBKDF2') {
      const iterations = options.pbkdf2Iterations || 1000
      strength += Math.min(iterations / 100, 5)
    }

    strength = Math.min(100, Math.round(strength))

    let level: 'low' | 'medium' | 'high' | 'maximum'
    if (strength < 30) level = 'low'
    else if (strength < 60) level = 'medium'
    else if (strength < 85) level = 'high'
    else level = 'maximum'

    return { strength, level, enabledFeatures, totalFeatures }
  }, [config.options])

  // Initialize options if empty
  useEffect(() => {
    if (Object.keys(config.options).length === 0) {
      onChange({ preset: config.preset, options: { ...presetDefaults[config.preset] } })
    }
  }, [config.preset])

  const handlePresetChange = (preset: typeof config.preset) => {
    onChange({ preset, options: { ...presetDefaults[preset] } })
  }

  const checkIfCustom = (options: Record<string, any>) => {
    const presets = ['basic', 'light', 'medium', 'heavy'] as const
    for (const preset of presets) {
      const defaults = presetDefaults[preset]
      const matches = Object.keys(defaults).every(key => {
        return options[key] === defaults[key as keyof typeof defaults]
      })
      if (matches) return preset
    }
    return 'custom' as const
  }

  const handleOptionChange = (key: string, value: any) => {
    const newOptions = {
      ...config.options,
      [key]: value
    }
    // Check if the new options match a preset, otherwise set to custom
    const matchingPreset = checkIfCustom(newOptions)
    onChange({
      preset: matchingPreset,
      options: newOptions
    })
  }

  const getCurrentValue = (key: string) => {
    return config.options[key] ?? presetDefaults[config.preset][key as keyof typeof presetDefaults[typeof config.preset]]
  }

  return (
    <div className="border border-vt-border rounded-lg bg-vt-bg-primary shadow-sm hover-lift transition-all duration-300">
      <div className="p-6">
        {/* Header */}
        <div className="mb-6">
          <h3 className="text-lg font-semibold text-vt-text-primary mb-1">Obfuscation Configuration</h3>
          <p className="text-sm text-vt-text-tertiary">Select a preset or customize your settings</p>
        </div>


        {/* Preset Selection */}
        <div className="mb-6">
          <label className="block text-sm font-semibold text-vt-text-secondary mb-3">Preset</label>
          <div className="grid grid-cols-2 md:grid-cols-5 gap-2">
            {(Object.keys(presetInfo) as Array<keyof typeof presetInfo>).map((preset) => {
              const info = presetInfo[preset]
              const isSelected = config.preset === preset
              const isRecommended = preset === 'medium'
              return (
                <button
                  key={preset}
                  onClick={() => handlePresetChange(preset)}
                  className={`relative px-3 py-2.5 rounded-lg border transition-all duration-200 text-sm font-medium text-left ${
                    isSelected
                      ? 'border-vt-primary bg-vt-primary text-white shadow-sm'
                      : 'border-vt-border bg-vt-bg-primary text-vt-text-primary hover:border-vt-primary/50 hover:bg-vt-bg-secondary'
                  }`}
                >
                  {isRecommended && !isSelected && (
                    <span className="absolute -top-2 -right-2 px-1.5 py-0.5 bg-vt-success text-white text-xs font-semibold rounded-full">
                      Recommended
                    </span>
                  )}
                  <div className="font-semibold">{info.name}</div>
                  <div className={`text-xs mt-0.5 ${isSelected ? 'text-white/80' : 'text-vt-text-tertiary'}`}>
                    {info.security} • {info.overhead}
                  </div>
                </button>
              )
            })}
          </div>
        </div>

        {/* Advanced Options Toggle */}
        <div className="border-t border-vt-border pt-6">
          <button
            onClick={() => setShowAdvanced(!showAdvanced)}
            className="flex items-center justify-between w-full p-3 bg-vt-bg-secondary hover:bg-vt-bg-tertiary rounded-lg transition-colors mb-4"
          >
            <span className="text-sm font-semibold text-vt-text-primary">
              {showAdvanced ? 'Hide' : 'Show'} Advanced Options
            </span>
            <svg
              className={`w-5 h-5 text-vt-text-tertiary transition-transform ${showAdvanced ? 'rotate-180' : ''}`}
              fill="none"
              stroke="currentColor"
              viewBox="0 0 24 24"
            >
              <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M19 9l-7 7-7-7" />
            </svg>
          </button>

          {showAdvanced && (
            <div className="space-y-6">
              {/* Tab Navigation */}
              <div className="flex border-b border-vt-border">
                {[
                  { id: 'core', label: 'Core Techniques' },
                  { id: 'advanced', label: 'Advanced Protection' },
                  { id: 'string', label: 'String Encryption' },
                  { id: 'params', label: 'Parameters' }
                ].map((tab) => (
                  <button
                    key={tab.id}
                    onClick={() => setActiveTab(tab.id as any)}
                    className={`px-4 py-2 text-sm font-medium border-b-2 transition-colors ${
                      activeTab === tab.id
                        ? 'border-vt-primary text-vt-primary'
                        : 'border-transparent text-vt-text-tertiary hover:text-vt-text-primary'
                    }`}
                  >
                    {tab.label}
                  </button>
                ))}
              </div>

              {/* Tab Content */}
              <div className="min-h-[300px]">
                {/* Core Techniques Tab */}
                {activeTab === 'core' && (
                  <div className="space-y-4">
                    <p className="text-xs text-vt-text-tertiary mb-4">
                      Basic obfuscation techniques that modify code structure
                    </p>
                    <div className="space-y-3">
                      {[
                        { key: 'enableControlFlowObfuscation', label: 'Control Flow Obfuscation', desc: 'Adds opaque predicates and dead code branches' },
                        { key: 'enableBogusCode', label: 'Bogus Code Insertion', desc: 'Inserts fake instructions to confuse analysis' },
                        { key: 'enableFakeLoops', label: 'Fake Loop Injection', desc: 'Adds never-executed loops for misdirection' },
                        { key: 'enableInstructionSubstitution', label: 'Instruction Substitution', desc: 'Replaces instructions with equivalent operations' },
                        { key: 'enableControlFlowFlattening', label: 'Control Flow Flattening', desc: 'Converts CFG into state machine (advanced)' },
                        { key: 'enableMBA', label: 'Mixed Boolean Arithmetic', desc: 'Replaces operations with complex MBA expressions' }
                      ].map(({ key, label, desc }) => {
                        const currentValue = getCurrentValue(key)
                        return (
                          <label key={key} className="flex items-start gap-3 p-3 rounded-lg hover:bg-vt-bg-secondary transition-colors cursor-pointer">
                            <div className="relative mt-0.5">
                              <input
                                type="checkbox"
                                checked={currentValue}
                                onChange={(e) => handleOptionChange(key, e.target.checked)}
                                className="w-4 h-4 appearance-none bg-white border-2 border-vt-border rounded focus:ring-2 focus:ring-vt-primary focus:ring-offset-0 checked:bg-vt-primary checked:border-vt-primary checked:hover:bg-vt-primary-hover checked:hover:border-vt-primary-hover transition-all cursor-pointer"
                              />
                              {currentValue && (
                                <svg className="absolute top-0 left-0 w-4 h-4 pointer-events-none" fill="none" stroke="white" strokeWidth="3" viewBox="0 0 24 24">
                                  <path strokeLinecap="round" strokeLinejoin="round" d="M5 13l4 4L19 7" />
                                </svg>
                              )}
                            </div>
                            <div className="flex-1">
                              <div className="text-sm font-medium text-vt-text-primary">{label}</div>
                              <div className="text-xs text-vt-text-tertiary mt-0.5">{desc}</div>
                            </div>
                          </label>
                        )
                      })}
                    </div>
                  </div>
                )}

                {/* Advanced Protection Tab */}
                {activeTab === 'advanced' && (
                  <div className="space-y-4">
                    <p className="text-xs text-vt-text-tertiary mb-4">
                      Advanced protection against reverse engineering and debugging
                    </p>
                    <div className="space-y-3">
                      {[
                        { key: 'enableAntiDebug', label: 'Anti-Debug Protection', desc: '8 detection methods: timing, hardware BPs, PEB checks, TLS callbacks' },
                        { key: 'enableIndirectCalls', label: 'Import Hiding', desc: 'Hides API calls using hash-based dynamic resolution' },
                        { key: 'enableAntiTamper', label: 'Anti-Tamper', desc: 'Detects code modifications via integrity checks' },
                        { key: 'enablePolymorphic', label: 'Polymorphic Code', desc: 'Generates multiple code variants with runtime selection' },
                        { key: 'enableMetamorphic', label: 'Metamorphic Engine', desc: 'Transforms code structure while maintaining functionality' }
                      ].map(({ key, label, desc }) => {
                        const currentValue = getCurrentValue(key)
                        return (
                          <label key={key} className="flex items-start gap-3 p-3 rounded-lg hover:bg-vt-bg-secondary transition-colors cursor-pointer">
                            <div className="relative mt-0.5">
                              <input
                                type="checkbox"
                                checked={currentValue}
                                onChange={(e) => handleOptionChange(key, e.target.checked)}
                                className="w-4 h-4 appearance-none bg-white border-2 border-vt-border rounded focus:ring-2 focus:ring-vt-primary focus:ring-offset-0 checked:bg-vt-primary checked:border-vt-primary checked:hover:bg-vt-primary-hover checked:hover:border-vt-primary-hover transition-all cursor-pointer"
                              />
                              {currentValue && (
                                <svg className="absolute top-0 left-0 w-4 h-4 pointer-events-none" fill="none" stroke="white" strokeWidth="3" viewBox="0 0 24 24">
                                  <path strokeLinecap="round" strokeLinejoin="round" d="M5 13l4 4L19 7" />
                                </svg>
                              )}
                            </div>
                            <div className="flex-1">
                              <div className="text-sm font-medium text-vt-text-primary">{label}</div>
                              <div className="text-xs text-vt-text-tertiary mt-0.5">{desc}</div>
                            </div>
                          </label>
                        )
                      })}
                    </div>
                  </div>
                )}

                {/* String Encryption Tab */}
                {activeTab === 'string' && (
                  <div className="space-y-4">
                    <p className="text-xs text-vt-text-tertiary mb-4">
                      Configure string encryption method and strength
                    </p>
                    
                    {/* Encryption Method */}
                    <div>
                      <label className="block text-sm font-semibold text-vt-text-secondary mb-3">
                        Encryption Method
                      </label>
                      <div className="space-y-2">
                        {[
                          { value: 'XOR_ROTATING', label: 'XOR Rotating (Legacy)', desc: 'Fast but weak, vulnerable to known-plaintext attacks' },
                          { value: 'RC4_SIMPLE', label: 'RC4 Simple', desc: 'Medium security, faster than PBKDF2' },
                          { value: 'RC4_PBKDF2', label: 'RC4 + PBKDF2 (Recommended)', desc: 'Strong encryption with key derivation from code hash' }
                        ].map(({ value, label, desc }) => {
                          const currentValue = getCurrentValue('stringEncryptionMethod')
                          return (
                            <label key={value} className="flex items-start gap-3 p-3 rounded-lg border border-vt-border hover:bg-vt-bg-secondary transition-colors cursor-pointer">
                              <div className="relative mt-0.5">
                                <input
                                  type="radio"
                                  name="stringEncryptionMethod"
                                  value={value}
                                  checked={currentValue === value}
                                  onChange={(e) => handleOptionChange('stringEncryptionMethod', e.target.value)}
                                  className="w-4 h-4 appearance-none bg-white border-2 border-vt-border rounded-full focus:ring-2 focus:ring-vt-primary focus:ring-offset-0 checked:bg-vt-primary checked:border-vt-primary checked:hover:bg-vt-primary-hover checked:hover:border-vt-primary-hover transition-all cursor-pointer"
                                />
                                {currentValue === value && (
                                  <div className="absolute top-1 left-1 w-2 h-2 bg-white rounded-full pointer-events-none" />
                                )}
                              </div>
                              <div className="flex-1">
                                <div className="text-sm font-medium text-vt-text-primary">{label}</div>
                                <div className="text-xs text-vt-text-tertiary mt-0.5">{desc}</div>
                              </div>
                            </label>
                          )
                        })}
                      </div>
                    </div>

                    {/* PBKDF2 Iterations (only show if RC4_PBKDF2 is selected) */}
                    {getCurrentValue('stringEncryptionMethod') === 'RC4_PBKDF2' && (
                      <div>
                        <div className="flex items-center justify-between mb-2">
                          <label className="text-sm font-semibold text-vt-text-secondary">
                            PBKDF2 Iterations
                          </label>
                          <span className="text-sm font-semibold text-vt-primary">
                            {getCurrentValue('pbkdf2Iterations')}
                          </span>
                        </div>
                        <input
                          type="range"
                          min="500"
                          max="5000"
                          step="100"
                          value={getCurrentValue('pbkdf2Iterations')}
                          onChange={(e) => handleOptionChange('pbkdf2Iterations', parseInt(e.target.value))}
                          className="w-full h-2 bg-vt-bg-tertiary rounded-lg appearance-none cursor-pointer accent-vt-primary"
                        />
                        <div className="flex justify-between text-xs text-vt-text-tertiary mt-1">
                          <span>500 (Fast)</span>
                          <span>5000 (Strongest)</span>
                        </div>
                        <p className="text-xs text-vt-text-tertiary mt-2">
                          Higher iterations = stronger encryption but slower decryption
                        </p>
                      </div>
                    )}

                    {/* Decryption Mode */}
                    <div>
                      <label className="flex items-start gap-3 p-3 rounded-lg hover:bg-vt-bg-secondary transition-colors cursor-pointer">
                        <div className="relative mt-0.5">
                          <input
                            type="checkbox"
                            checked={getCurrentValue('decryptStringsAtStartup')}
                            onChange={(e) => handleOptionChange('decryptStringsAtStartup', e.target.checked)}
                            className="w-4 h-4 appearance-none bg-white border-2 border-vt-border rounded focus:ring-2 focus:ring-vt-primary focus:ring-offset-0 checked:bg-vt-primary checked:border-vt-primary checked:hover:bg-vt-primary-hover checked:hover:border-vt-primary-hover transition-all cursor-pointer"
                          />
                          {getCurrentValue('decryptStringsAtStartup') && (
                            <svg className="absolute top-0 left-0 w-4 h-4 pointer-events-none" fill="none" stroke="white" strokeWidth="3" viewBox="0 0 24 24">
                              <path strokeLinecap="round" strokeLinejoin="round" d="M5 13l4 4L19 7" />
                            </svg>
                          )}
                        </div>
                        <div className="flex-1">
                          <div className="text-sm font-medium text-vt-text-primary">Decrypt Strings at Startup</div>
                          <div className="text-xs text-vt-text-tertiary mt-0.5">
                            If disabled, strings are decrypted lazily (on first access) for better security
                          </div>
                        </div>
                      </label>
                    </div>
                  </div>
                )}

                {/* Parameters Tab */}
                {activeTab === 'params' && (
                  <div className="space-y-5">
                    <p className="text-xs text-vt-text-tertiary mb-4">
                      Fine-tune obfuscation intensity and parameters
                    </p>
                    
                    {[
                      { key: 'obfuscationCycles', label: 'Obfuscation Cycles', min: 1, max: 10, step: 1, desc: 'Number of times to apply obfuscation passes' },
                      { key: 'bogusCodePercentage', label: 'Bogus Code Percentage', min: 0, max: 100, step: 5, desc: 'Percentage of fake instructions to insert', suffix: '%' },
                      { key: 'fakeLoopCount', label: 'Fake Loop Count', min: 0, max: 10, step: 1, desc: 'Number of fake loops per function' },
                      { key: 'mbaComplexity', label: 'MBA Complexity', min: 1, max: 10, step: 1, desc: 'Complexity of Mixed Boolean Arithmetic expressions' },
                      { key: 'polymorphicVariants', label: 'Polymorphic Variants', min: 1, max: 10, step: 1, desc: 'Number of code variants to generate' }
                    ].map(({ key, label, min, max, step, desc, suffix = '' }) => (
                      <div key={key}>
                        <div className="flex items-center justify-between mb-2">
                          <div>
                            <label className="text-sm font-semibold text-vt-text-secondary">
                              {label}
                            </label>
                            <p className="text-xs text-vt-text-tertiary mt-0.5">{desc}</p>
                          </div>
                          <span className="text-sm font-semibold text-vt-primary">
                            {getCurrentValue(key)}{suffix}
                          </span>
                        </div>
                        <input
                          type="range"
                          min={min}
                          max={max}
                          step={step}
                          value={getCurrentValue(key)}
                          onChange={(e) => handleOptionChange(key, parseInt(e.target.value))}
                          className="w-full h-2 bg-vt-bg-tertiary rounded-lg appearance-none cursor-pointer accent-vt-primary"
                        />
                        <div className="flex justify-between text-xs text-vt-text-tertiary mt-1">
                          <span>{min}{suffix}</span>
                          <span>{max}{suffix}</span>
                        </div>
                      </div>
                    ))}
                  </div>
                )}
              </div>
            </div>
          )}
        </div>
      </div>
    </div>
  )
}
