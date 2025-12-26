'use client'

import { useState, useEffect } from 'react'
import { useParams, useRouter } from 'next/navigation'
import Link from 'next/link'
import ObfuscationReport from '@/components/ObfuscationReport'
import CodeViewer from '@/components/CodeViewer'
import ProgressRing from '@/components/ProgressRing'
import StatCard from '@/components/StatCard'
import Logo from '@/components/Logo'
import ScrollReveal from '@/components/ScrollReveal'

interface JobStatus {
  status: 'processing' | 'completed' | 'failed'
  progress: number
  message: string
  obfuscatedIrPath?: string
  report?: {
    raw: string
    parsed: any
  }
  error?: string
}

export default function ResultsPage() {
  const params = useParams()
  const router = useRouter()
  const jobId = params.jobId as string
  const [status, setStatus] = useState<JobStatus | null>(null)
  const [polling, setPolling] = useState(true)

  useEffect(() => {
    if (!polling || !jobId) return

    const pollStatus = async () => {
      try {
        const response = await fetch(
          `${process.env.NEXT_PUBLIC_API_URL || 'http://localhost:3001'}/api/status/${jobId}`
        )

        if (!response.ok) {
          throw new Error('Failed to fetch status')
        }

        const data = await response.json()
        setStatus(data)

        if (data.status === 'completed' || data.status === 'failed') {
          setPolling(false)
        }
      } catch (error) {
        console.error('Error polling status:', error)
        setPolling(false)
      }
    }

    pollStatus()
    const interval = setInterval(pollStatus, 2000)

    return () => clearInterval(interval)
  }, [jobId, polling])

  if (!status) {
    return (
      <main className="min-h-screen bg-vt-bg-primary">
        <header className="border-b border-vt-border bg-vt-bg-secondary">
          <div className="vt-container py-4">
            <div className="flex items-center justify-between">
              <Link href="/" className="text-2xl font-semibold hover:text-vt-primary transition-colors">
                LLVM Code Obfuscator
              </Link>
              <nav className="flex items-center gap-4">
                <Link href="/" className="vt-link">Home</Link>
                <Link href="/presets" className="vt-link">Presets</Link>
              </nav>
            </div>
          </div>
        </header>
        <div className="vt-container py-8">
          <div className="vt-card text-center">
            <p>Loading job status...</p>
          </div>
        </div>
      </main>
    )
  }

  return (
    <main className="min-h-screen bg-vt-bg-primary">
      {/* Header - Sticky */}
      <header className="sticky top-0 z-50 border-b border-vt-border bg-vt-bg-primary/95 backdrop-blur-sm shadow-sm">
        <div className="vt-container py-4">
          <div className="flex items-center justify-between">
            <Logo size="md" showText={true} />
            <nav className="flex items-center gap-4">
              <Link href="/" className="text-sm font-medium text-vt-text-secondary hover:text-vt-primary transition-colors">
                Home
              </Link>
              <Link href="/presets" className="text-sm font-medium text-vt-text-secondary hover:text-vt-primary transition-colors">
                Presets
              </Link>
            </nav>
          </div>
        </div>
      </header>

      <div className="vt-container py-4 sm:py-8">
        <div className="mb-4 sm:mb-6">
          <Link href="/" className="text-vt-primary hover:underline mb-3 sm:mb-4 inline-block text-sm sm:text-base">
            ← Back to Home
          </Link>
          <h1 className="text-2xl sm:text-4xl font-bold">Obfuscation Results</h1>
          <p className="text-vt-text-tertiary mt-2 text-sm sm:text-base">Job ID: <span className="font-mono text-xs sm:text-sm">{jobId}</span></p>
        </div>

        {/* Status Card */}
        <ScrollReveal direction="up" delay={100}>
          <div className="border border-vt-border rounded-lg bg-vt-bg-primary shadow-sm mb-6 hover-lift transition-all duration-300">
          <div className="p-6">
            <div className="flex items-center justify-between mb-6">
              <h3 className="text-lg font-semibold text-vt-text-primary">Status</h3>
              {status.status === 'completed' && (
                <span className="px-3 py-1 text-xs font-semibold text-white bg-vt-success rounded-full">
                  Completed
                </span>
              )}
              {status.status === 'failed' && (
                <span className="px-3 py-1 text-xs font-semibold text-white bg-vt-danger rounded-full">
                  Failed
                </span>
              )}
              {status.status === 'processing' && (
                <span className="px-3 py-1 text-xs font-semibold text-vt-text-secondary bg-vt-bg-secondary rounded-full">
                  Processing
                </span>
              )}
            </div>

            {status.status === 'processing' && (
              <div className="flex flex-col md:flex-row items-center gap-8">
                {/* Progress Ring */}
                <div className="flex-shrink-0">
                  <ProgressRing 
                    progress={status.progress} 
                    size={120}
                    label="Processing"
                  />
                </div>
                
                {/* Progress Details */}
                <div className="flex-grow">
                  <div className="mb-4">
                    <div className="flex justify-between mb-3">
                      <span className="text-sm font-semibold text-vt-text-secondary">Progress</span>
                      <span className="text-lg font-bold text-vt-primary">{status.progress}%</span>
                    </div>
                    <div className="w-full h-3 bg-vt-bg-tertiary rounded-full overflow-hidden shadow-inner">
                      <div
                        className="h-full bg-gradient-to-r from-vt-primary to-vt-primary-light transition-all duration-300 rounded-full"
                        style={{ width: `${status.progress}%` }}
                      >
                        <div className="h-full w-full bg-gradient-to-r from-transparent to-white/30"></div>
                      </div>
                    </div>
                  </div>
                  <div className="space-y-2">
                    <p className="text-sm font-medium text-vt-text-primary">{status.message}</p>
                    <div className="flex items-center gap-2 text-xs text-vt-text-tertiary">
                      <div className="w-2 h-2 bg-vt-primary rounded-full animate-pulse"></div>
                      <span>Processing your file...</span>
                    </div>
                  </div>
                </div>
              </div>
            )}

            {status.status === 'completed' && (
              <div>
                <p className="text-sm text-vt-text-tertiary mb-6">{status.message}</p>

                {/* Success Stats */}
                {status.report?.parsed && (
                  <div className="grid grid-cols-1 md:grid-cols-3 gap-4 mb-6">
                    <StatCard
                      label="Transformations"
                      value={status.report.parsed.summary?.totalTransformations || 0}
                      icon={
                        <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                          <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M4 4v5h.582m15.356 2A8.001 8.001 0 004.582 9m0 0H9m11 11v-5h-.581m0 0a8.003 8.003 0 01-15.357-2m15.357 2H15" />
                        </svg>
                      }
                    />
                    <StatCard
                      label="Security Level"
                      value={status.report.parsed.summary?.securityLevel || 'High'}
                      icon={
                        <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                          <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 15v2m-6 4h12a2 2 0 002-2v-6a2 2 0 00-2-2H6a2 2 0 00-2 2v6a2 2 0 002 2zm10-10V7a4 4 0 00-8 0v4h8z" />
                        </svg>
                      }
                    />
                    <StatCard
                      label="Overhead"
                      value={status.report.parsed.summary?.estimatedOverhead || 'N/A'}
                      icon={
                        <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                          <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 19v-6a2 2 0 00-2-2H5a2 2 0 00-2 2v6a2 2 0 002 2h2a2 2 0 002-2zm0 0V9a2 2 0 012-2h2a2 2 0 012 2v10m-6 0a2 2 0 002 2h2a2 2 0 002-2m0 0V5a2 2 0 012-2h2a2 2 0 012 2v14a2 2 0 01-2 2h-2a2 2 0 01-2-2z" />
                        </svg>
                      }
                    />
                  </div>
                )}

                {status.obfuscatedIrPath && (
                  <div className="space-y-6">
                    {/* Download .ll File */}
                    <div className="border-2 border-vt-primary/30 rounded-xl bg-gradient-to-br from-vt-primary-subtle via-vt-bg-primary to-vt-bg-secondary p-6 shadow-lg">
                      <div className="flex items-start gap-5 mb-6">
                        <div className="w-16 h-16 bg-gradient-to-br from-vt-primary to-vt-primary-dark rounded-xl flex items-center justify-center flex-shrink-0 shadow-md border border-vt-primary/20">
                          <svg className="w-8 h-8 text-white" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                            <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M10 20l4-16m4 4l4 4-4 4M6 16l-4-4 4-4" />
                          </svg>
                        </div>
                        <div className="flex-grow min-w-0">
                          <div className="flex items-center gap-3 mb-2">
                            <p className="text-xl font-bold text-vt-text-primary">Obfuscated LLVM IR</p>
                            <span className="px-2.5 py-1 bg-vt-success/20 text-vt-success text-xs font-semibold rounded-full">
                              Ready
                            </span>
                          </div>
                          <p className="text-sm text-vt-text-tertiary font-mono mb-3">
                            {status.obfuscatedIrPath?.split(/[/\\]/).pop() || 'obfuscated.ll'}
                          </p>
                          <p className="text-sm text-vt-text-secondary">
                            Your code has been successfully obfuscated with world-class protection techniques. 
                            Download the LLVM IR file and compile it locally on your machine.
                          </p>
                        </div>
                      </div>
                      <a
                        href={`${process.env.NEXT_PUBLIC_API_URL || 'http://localhost:3001'}/api/download/${jobId}/ir`}
                        download={status.obfuscatedIrPath?.split(/[/\\]/).pop() || 'obfuscated.ll'}
                        className="inline-flex items-center justify-center gap-2 px-6 py-3.5 bg-vt-primary text-white text-base font-semibold rounded-lg hover:bg-vt-primary-hover transition-all duration-200 shadow-md hover:shadow-lg transform hover:-translate-y-0.5 active:translate-y-0"
                      >
                        <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                          <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M4 16v1a3 3 0 003 3h10a3 3 0 003-3v-1m-4-4l-4 4m0 0l-4-4m4 4V4" />
                        </svg>
                        Download LLVM IR (.ll)
                      </a>
                    </div>

                    {/* Compilation Instructions */}
                    <CompilationInstructions 
                      filename={status.obfuscatedIrPath?.split(/[/\\]/).pop() || 'obfuscated.ll'}
                    />
                  </div>
                )}
              </div>
            )}

            {status.status === 'failed' && (
              <div>
                <div className="border border-vt-danger/30 rounded-lg bg-vt-danger/5 p-4">
                  <div className="flex items-start gap-3">
                    <div className="w-5 h-5 text-vt-danger flex-shrink-0 mt-0.5">
                      <svg fill="currentColor" viewBox="0 0 20 20">
                        <path fillRule="evenodd" d="M10 18a8 8 0 100-16 8 8 0 000 16zM8.707 7.293a1 1 0 00-1.414 1.414L8.586 10l-1.293 1.293a1 1 0 101.414 1.414L10 11.414l1.293 1.293a1 1 0 001.414-1.414L11.414 10l1.293-1.293a1 1 0 00-1.414-1.414L10 8.586 8.707 7.293z" clipRule="evenodd" />
                      </svg>
                    </div>
                    <div className="flex-grow">
                      <p className="text-sm font-semibold text-vt-danger mb-1">Obfuscation Failed</p>
                      <p className="text-sm text-vt-text-secondary">{status.error || 'An error occurred during obfuscation'}</p>
                    </div>
                  </div>
                </div>
              </div>
            )}
          </div>
        </div>
        </ScrollReveal>

        {/* Obfuscated Code Display */}
        {status.status === 'completed' && status.obfuscatedIrPath && (
          <ScrollReveal direction="up" delay={200}>
            <div className="mb-6">
              <CodeViewer 
                jobId={jobId} 
                filename={status.obfuscatedIrPath?.split(/[/\\]/).pop()} 
              />
            </div>
          </ScrollReveal>
        )}

        {/* Report Display */}
        {status.status === 'completed' && status.report && (
          <ScrollReveal direction="up" delay={300}>
            <ObfuscationReport report={status.report} />
          </ScrollReveal>
        )}
      </div>
    </main>
  )
}

