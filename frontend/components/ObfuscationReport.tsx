'use client'

import { useState } from 'react'
import StatCard from './StatCard'
import ComparisonView from './ComparisonView'

interface ObfuscationReportProps {
  report: {
    raw: string
    parsed: {
      configuration?: Record<string, any>
      metrics?: Record<string, number>
      summary?: {
        totalTransformations: number
        estimatedOverhead: string
        securityLevel: string
      }
    }
  }
}

export default function ObfuscationReport({ report }: ObfuscationReportProps) {
  const [activeTab, setActiveTab] = useState<'summary' | 'metrics' | 'config' | 'raw'>('summary')
  const [expandedSections, setExpandedSections] = useState<Set<string>>(new Set())

  const toggleSection = (section: string) => {
    const newExpanded = new Set(expandedSections)
    if (newExpanded.has(section)) {
      newExpanded.delete(section)
    } else {
      newExpanded.add(section)
    }
    setExpandedSections(newExpanded)
  }

  const metrics = report.parsed?.metrics || {}
  const summary = report.parsed?.summary
  const config = report.parsed?.configuration || {}

  return (
    <div className="vt-card">
      <h3 className="vt-card-title mb-4">Obfuscation Report</h3>

      {/* Tabs */}
      <div className="vt-tabs mb-6">
        <button
          onClick={() => setActiveTab('summary')}
          className={`vt-tab ${activeTab === 'summary' ? 'vt-tab-active' : ''}`}
        >
          Summary
        </button>
        <button
          onClick={() => setActiveTab('metrics')}
          className={`vt-tab ${activeTab === 'metrics' ? 'vt-tab-active' : ''}`}
        >
          Metrics
        </button>
        <button
          onClick={() => setActiveTab('config')}
          className={`vt-tab ${activeTab === 'config' ? 'vt-tab-active' : ''}`}
        >
          Configuration
        </button>
        <button
          onClick={() => setActiveTab('raw')}
          className={`vt-tab ${activeTab === 'raw' ? 'vt-tab-active' : ''}`}
        >
          Raw Report
        </button>
      </div>

      {/* Summary Tab */}
      {activeTab === 'summary' && summary && (
        <div className="space-y-6">
          {/* Key Metrics */}
          <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
            <StatCard
              label="Total Transformations"
              value={summary.totalTransformations}
              icon={
                <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M4 4v5h.582m15.356 2A8.001 8.001 0 004.582 9m0 0H9m11 11v-5h-.581m0 0a8.003 8.003 0 01-15.357-2m15.357 2H15" />
                </svg>
              }
            />
            <StatCard
              label="Estimated Overhead"
              value={summary.estimatedOverhead}
              icon={
                <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 19v-6a2 2 0 00-2-2H5a2 2 0 00-2 2v6a2 2 0 002 2h2a2 2 0 002-2zm0 0V9a2 2 0 012-2h2a2 2 0 012 2v10m-6 0a2 2 0 002 2h2a2 2 0 002-2m0 0V5a2 2 0 012-2h2a2 2 0 012 2v14a2 2 0 01-2 2h-2a2 2 0 01-2-2z" />
                </svg>
              }
            />
            <StatCard
              label="Security Level"
              value={summary.securityLevel}
              icon={
                <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 15v2m-6 4h12a2 2 0 002-2v-6a2 2 0 00-2-2H6a2 2 0 00-2 2v6a2 2 0 002 2zm10-10V7a4 4 0 00-8 0v4h8z" />
                </svg>
              }
            />
          </div>

          {/* Comparison View */}
          {metrics && (
            <ComparisonView
              obfuscated={{
                fileSize: metrics.fileSize,
                instructionCount: metrics.instructionCount,
                functionCount: metrics.functionCount,
                transformations: summary.totalTransformations,
                securityLevel: summary.securityLevel
              }}
            />
          )}
        </div>
      )}

      {/* Metrics Tab */}
      {activeTab === 'metrics' && (
        <div className="space-y-6">
          {/* Visual Metrics Grid */}
          {metrics && Object.keys(metrics).length > 0 && (
            <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
              {Object.entries(metrics).map(([key, value]) => (
                <div key={key} className="border border-vt-border rounded-lg bg-vt-bg-secondary p-4">
                  <div className="text-xs text-vt-text-tertiary mb-1 uppercase tracking-wide">
                    {key.replace(/([A-Z])/g, ' $1').trim()}
                  </div>
                  <div className="text-xl font-bold text-vt-text-primary">
                    {typeof value === 'number' ? value.toLocaleString() : value}
                  </div>
                </div>
              ))}
            </div>
          )}

          {/* Detailed Table */}
          <div className="border border-vt-border rounded-lg overflow-hidden">
            <div className="overflow-x-auto">
              <table className="vt-table w-full">
                <thead className="bg-vt-bg-secondary">
                  <tr>
                    <th className="px-4 py-3 text-left">Metric</th>
                    <th className="px-4 py-3 text-right">Value</th>
                  </tr>
                </thead>
                <tbody>
                  {Object.entries(metrics).map(([key, value]) => (
                    <tr key={key}>
                      <td className="px-4 py-3 capitalize">{key.replace(/([A-Z])/g, ' $1').trim()}</td>
                      <td className="px-4 py-3 text-right font-medium">{value}</td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          </div>
        </div>
      )}

      {/* Configuration Tab */}
      {activeTab === 'config' && (
        <div className="space-y-2">
          <div className="vt-table">
            <thead>
              <tr>
                <th>Setting</th>
                <th>Value</th>
              </tr>
            </thead>
            <tbody>
              {Object.entries(config).map(([key, value]) => (
                <tr key={key}>
                  <td className="capitalize">{key.replace(/([A-Z])/g, ' $1').trim()}</td>
                  <td>
                    {typeof value === 'boolean' ? (
                      <span className={value ? 'vt-badge vt-badge-success' : 'vt-badge vt-badge-danger'}>
                        {value ? 'Enabled' : 'Disabled'}
                      </span>
                    ) : (
                      <span className="font-medium">{String(value)}</span>
                    )}
                  </td>
                </tr>
              ))}
            </tbody>
          </div>
        </div>
      )}

      {/* Raw Report Tab */}
      {activeTab === 'raw' && (
        <div className="space-y-4">
          <div className="flex items-center justify-between">
            <p className="text-sm text-vt-text-tertiary">Raw report data in JSON format</p>
            <button
              onClick={() => {
                const blob = new Blob([report.raw], { type: 'application/json' })
                const url = URL.createObjectURL(blob)
                const a = document.createElement('a')
                a.href = url
                a.download = 'obfuscation-report.json'
                a.click()
                URL.revokeObjectURL(url)
              }}
              className="px-4 py-2 text-sm font-medium text-vt-primary hover:bg-vt-primary-subtle rounded-lg transition-colors"
            >
              Export JSON
            </button>
          </div>
          <div className="bg-vt-bg-secondary rounded-lg p-4 border border-vt-border">
            <pre className="text-xs font-mono text-vt-text-secondary whitespace-pre-wrap overflow-x-auto max-h-96 overflow-y-auto">
              {report.raw}
            </pre>
          </div>
        </div>
      )}
    </div>
  )
}


