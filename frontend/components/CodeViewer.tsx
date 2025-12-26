'use client'

import { useState, useEffect } from 'react'

interface CodeViewerProps {
  jobId: string
  filename?: string
}

export default function CodeViewer({ jobId, filename }: CodeViewerProps) {
  const [code, setCode] = useState<string | null>(null)
  const [loading, setLoading] = useState(true)
  const [error, setError] = useState<string | null>(null)

  useEffect(() => {
    const fetchCode = async () => {
      try {
        setLoading(true)
        setError(null)
        const response = await fetch(
          `${process.env.NEXT_PUBLIC_API_URL || 'http://localhost:3001'}/api/view/${jobId}/ir`
        )

        if (!response.ok) {
          const errorData = await response.json().catch(() => ({ error: 'Unknown error' }))
          throw new Error(errorData.error || `HTTP ${response.status}: Failed to fetch obfuscated code`)
        }

        const data = await response.json()
        setCode(data.content)
        setError(null)
      } catch (err) {
        const errorMessage = err instanceof Error ? err.message : 'Failed to load code'
        console.error('Error fetching obfuscated IR:', err)
        setError(errorMessage)
        setCode(null)
      } finally {
        setLoading(false)
      }
    }

    fetchCode()
  }, [jobId])

  if (loading) {
    return (
      <div className="border border-vt-border rounded-lg bg-vt-bg-primary p-6">
        <p className="text-sm text-vt-text-tertiary">Loading obfuscated code...</p>
      </div>
    )
  }

  if (error) {
    return (
      <div className="border border-vt-border rounded-lg bg-vt-bg-primary p-6">
        <div className="flex items-start gap-3">
          <div className="w-5 h-5 text-vt-danger flex-shrink-0 mt-0.5">
            <svg fill="currentColor" viewBox="0 0 20 20">
              <path fillRule="evenodd" d="M10 18a8 8 0 100-16 8 8 0 000 16zM8.707 7.293a1 1 0 00-1.414 1.414L8.586 10l-1.293 1.293a1 1 0 101.414 1.414L10 11.414l1.293 1.293a1 1 0 001.414-1.414L11.414 10l1.293-1.293a1 1 0 00-1.414-1.414L10 8.586 8.707 7.293z" clipRule="evenodd" />
            </svg>
          </div>
          <div className="flex-grow">
            <p className="text-sm font-semibold text-vt-danger mb-1">Failed to Load Obfuscated Code</p>
            <p className="text-xs text-vt-text-tertiary">{error}</p>
            <p className="text-xs text-vt-text-tertiary mt-2">
              The obfuscated IR file may not be available. Please check if the obfuscation completed successfully.
            </p>
          </div>
        </div>
      </div>
    )
  }

  return (
    <div className="border border-vt-border rounded-lg bg-vt-bg-primary shadow-sm">
      <div className="border-b border-vt-border p-4 flex items-center justify-between bg-vt-bg-secondary">
        <div>
          <h3 className="text-base font-semibold text-vt-text-primary mb-1">
            Obfuscated LLVM IR Code
          </h3>
          <p className="text-xs text-vt-text-tertiary">
            {filename || 'obfuscated.ll'} • Intermediate representation after obfuscation
          </p>
        </div>
        <a
          href={`${process.env.NEXT_PUBLIC_API_URL || 'http://localhost:3001'}/api/download/${jobId}/ir`}
          download={filename || 'obfuscated.ll'}
          className="px-3 py-1.5 text-xs font-semibold bg-vt-primary text-white rounded hover:bg-vt-primary-hover transition-colors inline-flex items-center gap-2"
        >
          <svg className="w-3.5 h-3.5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
            <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M4 16v1a3 3 0 003 3h10a3 3 0 003-3v-1m-4-4l-4 4m0 0l-4-4m4 4V4" />
          </svg>
          Download IR
        </a>
      </div>
      <div className="p-4 overflow-auto max-h-[600px] bg-vt-bg-secondary">
        <pre className="text-xs font-mono text-vt-text-secondary leading-relaxed whitespace-pre-wrap break-words">
          <code>{code}</code>
        </pre>
      </div>
    </div>
  )
}

