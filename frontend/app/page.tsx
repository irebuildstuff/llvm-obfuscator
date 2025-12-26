'use client'

import { useState } from 'react'
import { useRouter } from 'next/navigation'
import Link from 'next/link'
import FileUpload from '@/components/FileUpload'
import ObfuscationConfig from '@/components/ObfuscationConfig'
import Logo from '@/components/Logo'
import ScrollReveal from '@/components/ScrollReveal'
import GradientBackground from '@/components/GradientBackground'
import AnimatedCounter from '@/components/AnimatedCounter'
import Tooltip from '@/components/Tooltip'
import Popover from '@/components/Popover'

export default function Home() {
  const router = useRouter()
  const [uploadedFile, setUploadedFile] = useState<File | null>(null)
  const [config, setConfig] = useState({
    preset: 'medium' as 'basic' | 'light' | 'medium' | 'heavy' | 'custom',
    options: {} as Record<string, any>
  })
  const [isProcessing, setIsProcessing] = useState(false)

  const handleFileUpload = (file: File | null) => {
    setUploadedFile(file)
  }

  const handleConfigChange = (newConfig: typeof config) => {
    setConfig(newConfig)
  }

  const handleStartObfuscation = async () => {
    if (!uploadedFile) return

    setIsProcessing(true)

    try {
      const formData = new FormData()
      formData.append('file', uploadedFile)
      formData.append('preset', config.preset)
      formData.append('options', JSON.stringify(config.options))

      const response = await fetch(`${process.env.NEXT_PUBLIC_API_URL || 'http://localhost:3001'}/api/obfuscate`, {
        method: 'POST',
        body: formData,
      })

      if (!response.ok) {
        throw new Error('Failed to start obfuscation')
      }

      const data = await response.json()
      router.push(`/results/${data.jobId}`)
    } catch (error) {
      console.error('Error starting obfuscation:', error)
      alert('Failed to start obfuscation. Please try again.')
      setIsProcessing(false)
    }
  }

  return (
    <>
      <main className="min-h-screen bg-vt-bg-primary relative overflow-hidden">
        {/* Animated Background Gradient */}
        <GradientBackground variant="animated" className="opacity-30" />
        
        {/* Top Navigation Bar - Premium Sticky Header */}
        <header className="sticky top-0 z-50 border-b border-vt-border bg-vt-bg-primary/95 backdrop-blur-sm shadow-sm relative">
        <div className="vt-container">
          <div className="flex items-center justify-between py-4">
            {/* Logo */}
            <Logo size="md" showText={true} />

            {/* Right Side Icons */}
            <nav className="flex items-center gap-2">
              <Tooltip content="Upload a new file" position="bottom">
                <button 
                  className="p-2 rounded hover:bg-vt-bg-secondary transition-colors"
                  onClick={() => {
                    const fileInput = document.querySelector('input[type="file"]') as HTMLInputElement
                    fileInput?.click()
                  }}
                >
                  <svg className="w-5 h-5 text-vt-text-secondary" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M7 16a4 4 0 01-.88-7.903A5 5 0 1115.9 6L16 6a5 5 0 011 9.9M15 13l-3-3m0 0l-3 3m3-3v12" />
                  </svg>
                </button>
              </Tooltip>

              <Popover
                trigger={
                  <button className="p-2 rounded hover:bg-vt-bg-secondary transition-colors">
                    <svg className="w-5 h-5 text-vt-text-secondary" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M8 12h.01M12 12h.01M16 12h.01M21 12c0 4.418-4.03 8-9 8a9.863 9.863 0 01-4.255-.949L3 20l1.395-3.72C3.512 15.042 3 13.574 3 12c0-4.418 4.03-8 9-8s9 3.582 9 8z" />
                    </svg>
                  </button>
                }
                position="bottom-right"
              >
                <div className="p-4">
                  <h3 className="font-semibold text-vt-text-primary mb-3">Messages</h3>
                  <div className="text-sm text-vt-text-tertiary text-center py-8">
                    <svg className="w-12 h-12 mx-auto mb-3 text-vt-text-tertiary opacity-50" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M8 12h.01M12 12h.01M16 12h.01M21 12c0 4.418-4.03 8-9 8a9.863 9.863 0 01-4.255-.949L3 20l1.395-3.72C3.512 15.042 3 13.574 3 12c0-4.418 4.03-8 9-8s9 3.582 9 8z" />
                    </svg>
                    <p>No new messages</p>
                  </div>
                </div>
              </Popover>

              <Popover
                trigger={
                  <button className="p-2 rounded hover:bg-vt-bg-secondary transition-colors relative">
                    <svg className="w-5 h-5 text-vt-text-secondary" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M15 17h5l-1.405-1.405A2.032 2.032 0 0118 14.158V11a6.002 6.002 0 00-4-5.659V5a2 2 0 10-4 0v.341C7.67 6.165 6 8.388 6 11v3.159c0 .538-.214 1.055-.595 1.436L4 17h5m6 0v1a3 3 0 11-6 0v-1m6 0H9" />
                    </svg>
                    <span className="absolute top-1 right-1 w-2 h-2 bg-red-500 rounded-full"></span>
                  </button>
                }
                position="bottom-right"
              >
                <div className="p-4">
                  <h3 className="font-semibold text-vt-text-primary mb-3">Notifications</h3>
                  <div className="text-sm text-vt-text-tertiary text-center py-8">
                    <svg className="w-12 h-12 mx-auto mb-3 text-vt-text-tertiary opacity-50" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M15 17h5l-1.405-1.405A2.032 2.032 0 0118 14.158V11a6.002 6.002 0 00-4-5.659V5a2 2 0 10-4 0v.341C7.67 6.165 6 8.388 6 11v3.159c0 .538-.214 1.055-.595 1.436L4 17h5m6 0v1a3 3 0 11-6 0v-1m6 0H9" />
                    </svg>
                    <p>No new notifications</p>
                  </div>
                </div>
              </Popover>

              <Popover
                trigger={
                  <button className="p-2 rounded hover:bg-vt-bg-secondary transition-colors">
                    <svg className="w-5 h-5 text-vt-text-secondary" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M8.228 9c.549-1.165 2.03-2 3.772-2 2.21 0 4 1.343 4 3 0 1.4-1.278 2.575-3.006 2.907-.542.104-.994.54-.994 1.093m0 3h.01M21 12a9 9 0 11-18 0 9 9 0 0118 0z" />
                    </svg>
                  </button>
                }
                position="bottom-right"
              >
                <div className="p-4 max-w-sm">
                  <h3 className="font-semibold text-vt-text-primary mb-4">Help & Support</h3>
                  <div className="space-y-3">
                    <Link href="/docs" className="flex items-center gap-3 p-3 rounded-lg hover:bg-vt-bg-secondary transition-colors group">
                      <svg className="w-5 h-5 text-vt-text-tertiary group-hover:text-vt-primary" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                        <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 6.253v13m0-13C10.832 5.477 9.246 5 7.5 5S4.168 5.477 3 6.253v13C4.168 18.477 5.754 18 7.5 18s3.332.477 4.5 1.253m0-13C13.168 5.477 14.754 5 16.5 5c1.747 0 3.332.477 4.5 1.253v13C19.832 18.477 18.247 18 16.5 18c-1.746 0-3.332.477-4.5 1.253" />
                      </svg>
                      <div>
                        <div className="font-medium text-vt-text-primary text-sm">Documentation</div>
                        <div className="text-xs text-vt-text-tertiary">Learn how to use the obfuscator</div>
                      </div>
                    </Link>
                    <Link href="/contact" className="flex items-center gap-3 p-3 rounded-lg hover:bg-vt-bg-secondary transition-colors group">
                      <svg className="w-5 h-5 text-vt-text-tertiary group-hover:text-vt-primary" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                        <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M3 8l7.89 5.26a2 2 0 002.22 0L21 8M5 19h14a2 2 0 002-2V7a2 2 0 00-2-2H5a2 2 0 00-2 2v10a2 2 0 002 2z" />
                      </svg>
                      <div>
                        <div className="font-medium text-vt-text-primary text-sm">Contact Support</div>
                        <div className="text-xs text-vt-text-tertiary">Get help from our team</div>
                      </div>
                    </Link>
                    <Link href="/presets" className="flex items-center gap-3 p-3 rounded-lg hover:bg-vt-bg-secondary transition-colors group">
                      <svg className="w-5 h-5 text-vt-text-tertiary group-hover:text-vt-primary" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                        <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 12l2 2 4-4m5.618-4.016A11.955 11.955 0 0112 2.944a11.955 11.955 0 01-8.618 3.04A12.02 12.02 0 003 9c0 5.591 3.824 10.29 9 11.622 5.176-1.332 9-6.03 9-11.622 0-1.042-.133-2.052-.382-3.016z" />
                      </svg>
                      <div>
                        <div className="font-medium text-vt-text-primary text-sm">Obfuscation Presets</div>
                        <div className="text-xs text-vt-text-tertiary">View available presets</div>
                      </div>
                    </Link>
                    <div className="border-t border-vt-border pt-3 mt-3">
                      <Link href="/terms" className="flex items-center gap-3 p-3 rounded-lg hover:bg-vt-bg-secondary transition-colors group">
                        <svg className="w-5 h-5 text-vt-text-tertiary group-hover:text-vt-primary" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                          <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 12h6m-6 4h6m2 5H7a2 2 0 01-2-2V5a2 2 0 012-2h5.586a1 1 0 01.707.293l5.414 5.414a1 1 0 01.293.707V19a2 2 0 01-2 2z" />
                        </svg>
                        <div>
                          <div className="font-medium text-vt-text-primary text-sm">Terms & Privacy</div>
                          <div className="text-xs text-vt-text-tertiary">Legal information</div>
                        </div>
                      </Link>
                    </div>
                  </div>
                </div>
              </Popover>
              <div className="h-6 w-px bg-vt-border mx-2"></div>
              <Link href="/sign-in" className="px-4 py-2 text-sm font-medium text-vt-text-primary hover:text-vt-primary transition-colors">
                Sign in
              </Link>
              <Link href="/sign-up" className="px-5 py-2.5 bg-vt-primary text-white rounded-lg hover:bg-vt-primary-hover transition-all duration-200 text-sm font-semibold shadow-sm hover:shadow-md">
                Sign up
              </Link>
            </nav>
          </div>
        </div>
      </header>

      {/* Main Content - Premium Hero Section */}
      <div className="vt-container py-16 sm:py-20 relative">
        <GradientBackground variant="animated" />
        
        {/* Hero Section */}
        <div className="text-center mb-16 max-w-4xl mx-auto relative z-10">
          {/* Trust Indicators */}
          <ScrollReveal direction="fade" delay={100}>
            <div className="flex items-center justify-center gap-6 mb-8 flex-wrap">
              <div className="flex items-center gap-2 text-sm text-vt-text-tertiary">
                <svg className="w-5 h-5 text-vt-success" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 12l2 2 4-4m6 2a9 9 0 11-18 0 9 9 0 0118 0z" />
                </svg>
                <span>World-Class Protection</span>
              </div>
              <div className="flex items-center gap-2 text-sm text-vt-text-tertiary">
                <svg className="w-5 h-5 text-vt-primary" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M13 10V3L4 14h7v7l9-11h-7z" />
                </svg>
                <span>Lightning Fast</span>
              </div>
              <div className="flex items-center gap-2 text-sm text-vt-text-tertiary">
                <svg className="w-5 h-5 text-vt-info" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 15v2m-6 4h12a2 2 0 002-2v-6a2 2 0 00-2-2H6a2 2 0 00-2 2v6a2 2 0 002 2zm10-10V7a4 4 0 00-8 0v4h8z" />
                </svg>
                <span>Enterprise Grade</span>
              </div>
            </div>
          </ScrollReveal>

          {/* Main Headline */}
          <ScrollReveal direction="up" delay={200}>
            <h1 className="text-4xl sm:text-5xl md:text-6xl font-bold text-vt-text-primary mb-6 tracking-tight leading-tight">
              World-Class Code Protection
              <span className="block text-vt-primary mt-2">For Your C/C++ Applications</span>
            </h1>
          </ScrollReveal>
          
          {/* Value Proposition */}
          <ScrollReveal direction="up" delay={300}>
            <p className="text-lg sm:text-xl text-vt-text-secondary max-w-2xl mx-auto leading-relaxed mb-8 px-4">
              Protect your intellectual property with advanced obfuscation techniques. 
              Transform your source code into hardened executables that resist reverse engineering.
            </p>
          </ScrollReveal>

          {/* Statistics */}
          <ScrollReveal direction="up" delay={400}>
            <div className="grid grid-cols-3 gap-8 max-w-2xl mx-auto mb-12">
              <div className="hover-lift p-4 rounded-lg">
                <div className="text-3xl font-bold text-vt-primary mb-1">
                  <AnimatedCounter value={16} suffix="+" />
                </div>
                <div className="text-sm text-vt-text-tertiary">Obfuscation Techniques</div>
              </div>
              <div className="hover-lift p-4 rounded-lg">
                <div className="text-3xl font-bold text-vt-primary mb-1">RC4+PBKDF2</div>
                <div className="text-sm text-vt-text-tertiary">Strong Encryption</div>
              </div>
              <div className="hover-lift p-4 rounded-lg">
                <div className="text-3xl font-bold text-vt-primary mb-1">
                  <AnimatedCounter value={100} suffix="%" />
                </div>
                <div className="text-sm text-vt-text-tertiary">Open Source</div>
              </div>
            </div>
          </ScrollReveal>
        </div>

        {/* File Upload Section */}
        <ScrollReveal direction="up" delay={500}>
          <div className="max-w-3xl mx-auto relative z-10">
            <FileUpload onFileSelect={handleFileUpload} />
          
            {/* Configuration Section */}
            {uploadedFile && (
              <ScrollReveal direction="up" delay={100}>
                <div className="mt-6">
                  <ObfuscationConfig
                    config={config}
                    onChange={handleConfigChange}
                  />
                </div>
              </ScrollReveal>
            )}

            {/* Start Button */}
            {uploadedFile && (
              <ScrollReveal direction="up" delay={200}>
                <div className="text-center mt-8">
                  <button
                    onClick={handleStartObfuscation}
                    disabled={isProcessing}
                    className="px-10 py-3.5 bg-vt-primary text-white rounded-lg font-semibold text-base hover:bg-vt-primary-hover transition-all duration-200 shadow-md hover:shadow-lg hover-lift glow-on-hover disabled:opacity-50 disabled:cursor-not-allowed disabled:hover:shadow-md disabled:hover:transform-none"
                  >
                    {isProcessing ? 'Starting...' : 'Start Obfuscation'}
                  </button>
                </div>
              </ScrollReveal>
            )}

            {/* Legal Disclaimer */}
            <ScrollReveal direction="fade" delay={600}>
              <p className="text-sm text-vt-text-tertiary text-center mt-10 max-w-3xl mx-auto leading-relaxed px-4">
                By submitting data above, you are agreeing to our{' '}
                <Link href="/terms" className="text-vt-primary hover:underline font-medium">Terms of Service</Link>
                {' '}and{' '}
                <Link href="/privacy" className="text-vt-primary hover:underline font-medium">Privacy Notice</Link>
                , and to the <strong className="font-semibold">sharing of your Sample submission with the security community</strong>.
                Please do not submit any personal information; we are not responsible for the contents of your submission.{' '}
                <Link href="/learn-more" className="text-vt-primary hover:underline font-medium">Learn more.</Link>
              </p>
            </ScrollReveal>
          </div>
        </ScrollReveal>
      </div>

      {/* Footer - Enhanced */}
      <footer className="border-t border-vt-border bg-vt-bg-secondary mt-24 py-12">
        <div className="vt-container">
          <div className="grid grid-cols-1 md:grid-cols-4 gap-8 mb-8">
            {/* Brand */}
            <div>
              <Logo size="sm" showText={true} className="mb-4" />
              <p className="text-sm text-vt-text-tertiary leading-relaxed">
                World-class code obfuscation for C/C++ applications. Protect your intellectual property with advanced techniques.
              </p>
            </div>

            {/* Features */}
            <div>
              <h4 className="font-semibold text-vt-text-primary mb-4">Features</h4>
              <ul className="space-y-2 text-sm text-vt-text-tertiary">
                <li><Link href="/presets" className="hover:text-vt-primary transition-colors">Obfuscation Presets</Link></li>
                <li><Link href="/features" className="hover:text-vt-primary transition-colors">Advanced Protection</Link></li>
                <li><Link href="/techniques" className="hover:text-vt-primary transition-colors">All Techniques</Link></li>
                <li><Link href="/comparison" className="hover:text-vt-primary transition-colors">Compare Results</Link></li>
              </ul>
            </div>

            {/* Resources */}
            <div>
              <h4 className="font-semibold text-vt-text-primary mb-4">Resources</h4>
              <ul className="space-y-2 text-sm text-vt-text-tertiary">
                <li><Link href="/docs" className="hover:text-vt-primary transition-colors">Documentation</Link></li>
                <li><Link href="/api" className="hover:text-vt-primary transition-colors">API Reference</Link></li>
                <li><Link href="/examples" className="hover:text-vt-primary transition-colors">Examples</Link></li>
                <li><Link href="/blog" className="hover:text-vt-primary transition-colors">Blog</Link></li>
              </ul>
            </div>

            {/* Company */}
            <div>
              <h4 className="font-semibold text-vt-text-primary mb-4">Company</h4>
              <ul className="space-y-2 text-sm text-vt-text-tertiary">
                <li><Link href="/about" className="hover:text-vt-primary transition-colors">About</Link></li>
                <li><Link href="/terms" className="hover:text-vt-primary transition-colors">Terms of Service</Link></li>
                <li><Link href="/privacy" className="hover:text-vt-primary transition-colors">Privacy Policy</Link></li>
                <li><Link href="/contact" className="hover:text-vt-primary transition-colors">Contact</Link></li>
              </ul>
            </div>
          </div>

          {/* Bottom Bar */}
          <div className="border-t border-vt-border pt-8 flex flex-col sm:flex-row items-center justify-between gap-4">
            <p className="text-sm text-vt-text-tertiary">
              © 2025 LLVM Obfuscator. Open source and free to use.
            </p>
            <div className="flex items-center gap-6">
              <Link href="/api" className="text-sm text-vt-text-tertiary hover:text-vt-primary transition-colors">
                API
              </Link>
              <Link href="/status" className="text-sm text-vt-text-tertiary hover:text-vt-primary transition-colors">
                Status
              </Link>
              <div className="flex items-center gap-2 text-sm text-vt-text-tertiary">
                <div className="w-2 h-2 bg-vt-success rounded-full animate-pulse"></div>
                <span>All systems operational</span>
              </div>
            </div>
          </div>
        </div>
      </footer>
      </main>
    </>
  )
}
