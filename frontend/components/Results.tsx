'use client'

import { useState, useEffect } from 'react'
import ObfuscationReport from './ObfuscationReport'

interface ResultsProps {
  jobId: string
}

interface JobStatus {
  status: 'processing' | 'completed' | 'failed'
  progress: number
  message: string
  executableUrl?: string
  report?: {
    raw: string
    parsed: any
  }
  error?: string
}

export default function Results({ jobId }: ResultsProps) {
  const [status, setStatus] = useState<JobStatus | null>(null)
  const [polling, setPolling] = useState(true)

  useEffect(() => {
    if (!polling) return

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
    const interval = setInterval(pollStatus, 2000) // Poll every 2 seconds

    return () => clearInterval(interval)
  }, [jobId, polling])

  if (!status) {
    return (
      <div className="vt-card">
        <p>Loading job status...</p>
      </div>
    )
  }

  return (
    <div className="space-y-6">
      {/* Status Card */}
      <div className="vt-card">
        <h3 className="vt-card-title mb-4">Obfuscation Status</h3>

        {status.status === 'processing' && (
          <div>
            <div className="mb-4">
              <div className="flex justify-between mb-2">
                <span className="text-sm text-vt-text-tertiary">Progress</span>
                <span className="text-sm font-medium">{status.progress}%</span>
              </div>
              <div className="vt-progress">
                <div
                  className="vt-progress-bar"
                  style={{ width: `${status.progress}%` }}
                />
              </div>
            </div>
            <p className="text-vt-text-tertiary">{status.message}</p>
          </div>
        )}

        {status.status === 'completed' && (
          <div>
            <div className="vt-badge vt-badge-success mb-4">Completed</div>
            <p className="text-vt-text-tertiary mb-4">{status.message}</p>

            {status.executableUrl && (
              <a
                href={`${process.env.NEXT_PUBLIC_API_URL || 'http://localhost:3001'}/api/download/${jobId}/executable`}
                download
                className="vt-btn inline-block"
              >
                Download Executable
              </a>
            )}
          </div>
        )}

        {status.status === 'failed' && (
          <div>
            <div className="vt-badge vt-badge-danger mb-4">Failed</div>
            <p className="text-vt-danger">{status.error || 'Obfuscation failed'}</p>
          </div>
        )}
      </div>

      {/* Report Display */}
      {status.status === 'completed' && status.report && (
        <ObfuscationReport report={status.report} />
      )}
    </div>
  )
}


