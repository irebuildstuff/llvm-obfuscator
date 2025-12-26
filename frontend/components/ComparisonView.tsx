'use client'

import StatCard from './StatCard'

interface ComparisonViewProps {
  original?: {
    fileSize?: number
    instructionCount?: number
    functionCount?: number
  }
  obfuscated?: {
    fileSize?: number
    instructionCount?: number
    functionCount?: number
    transformations?: number
    securityLevel?: string
  }
}

export default function ComparisonView({ original, obfuscated }: ComparisonViewProps) {
  if (!original || !obfuscated) {
    return (
      <div className="border border-vt-border rounded-xl bg-vt-bg-primary p-8 text-center">
        <p className="text-vt-text-tertiary">Comparison data not available</p>
      </div>
    )
  }

  const calculateChange = (original: number, obfuscated: number) => {
    const change = ((obfuscated - original) / original) * 100
    return {
      value: change,
      isPositive: change > 0,
      formatted: `${change > 0 ? '+' : ''}${change.toFixed(1)}%`
    }
  }

  const fileSizeChange = original.fileSize && obfuscated.fileSize 
    ? calculateChange(original.fileSize, obfuscated.fileSize)
    : null

  const instructionChange = original.instructionCount && obfuscated.instructionCount
    ? calculateChange(original.instructionCount, obfuscated.instructionCount)
    : null

  return (
    <div className="border border-vt-border rounded-xl bg-vt-bg-primary p-6">
      <h3 className="text-lg font-semibold text-vt-text-primary mb-6">Before & After Comparison</h3>
      
      <div className="grid grid-cols-1 md:grid-cols-2 gap-6 mb-6">
        {/* Original */}
        <div className="space-y-4">
          <h4 className="text-sm font-semibold text-vt-text-secondary uppercase tracking-wide">Original</h4>
          {original.fileSize && (
            <div className="p-4 bg-vt-bg-secondary rounded-lg">
              <div className="text-xs text-vt-text-tertiary mb-1">File Size</div>
              <div className="text-lg font-semibold text-vt-text-primary">
                {original.fileSize < 1024 
                  ? `${original.fileSize} B`
                  : original.fileSize < 1024 * 1024
                  ? `${(original.fileSize / 1024).toFixed(2)} KB`
                  : `${(original.fileSize / (1024 * 1024)).toFixed(2)} MB`}
              </div>
            </div>
          )}
          {original.instructionCount && (
            <div className="p-4 bg-vt-bg-secondary rounded-lg">
              <div className="text-xs text-vt-text-tertiary mb-1">Instructions</div>
              <div className="text-lg font-semibold text-vt-text-primary">
                {original.instructionCount.toLocaleString()}
              </div>
            </div>
          )}
          {original.functionCount && (
            <div className="p-4 bg-vt-bg-secondary rounded-lg">
              <div className="text-xs text-vt-text-tertiary mb-1">Functions</div>
              <div className="text-lg font-semibold text-vt-text-primary">
                {original.functionCount}
              </div>
            </div>
          )}
        </div>

        {/* Obfuscated */}
        <div className="space-y-4">
          <h4 className="text-sm font-semibold text-vt-text-secondary uppercase tracking-wide">Obfuscated</h4>
          {obfuscated.fileSize && (
            <div className="p-4 bg-vt-primary-subtle rounded-lg border border-vt-primary/20">
              <div className="flex items-center justify-between mb-1">
                <div className="text-xs text-vt-text-tertiary">File Size</div>
                {fileSizeChange && (
                  <span className={`text-xs font-semibold ${
                    fileSizeChange.isPositive ? 'text-vt-warning' : 'text-vt-success'
                  }`}>
                    {fileSizeChange.formatted}
                  </span>
                )}
              </div>
              <div className="text-lg font-semibold text-vt-text-primary">
                {obfuscated.fileSize < 1024 
                  ? `${obfuscated.fileSize} B`
                  : obfuscated.fileSize < 1024 * 1024
                  ? `${(obfuscated.fileSize / 1024).toFixed(2)} KB`
                  : `${(obfuscated.fileSize / (1024 * 1024)).toFixed(2)} MB`}
              </div>
            </div>
          )}
          {obfuscated.instructionCount && (
            <div className="p-4 bg-vt-primary-subtle rounded-lg border border-vt-primary/20">
              <div className="flex items-center justify-between mb-1">
                <div className="text-xs text-vt-text-tertiary">Instructions</div>
                {instructionChange && (
                  <span className={`text-xs font-semibold ${
                    instructionChange.isPositive ? 'text-vt-warning' : 'text-vt-success'
                  }`}>
                    {instructionChange.formatted}
                  </span>
                )}
              </div>
              <div className="text-lg font-semibold text-vt-text-primary">
                {obfuscated.instructionCount.toLocaleString()}
              </div>
            </div>
          )}
          {obfuscated.functionCount && (
            <div className="p-4 bg-vt-primary-subtle rounded-lg border border-vt-primary/20">
              <div className="text-xs text-vt-text-tertiary mb-1">Functions</div>
              <div className="text-lg font-semibold text-vt-text-primary">
                {obfuscated.functionCount}
              </div>
            </div>
          )}
        </div>
      </div>

      {/* Summary Stats */}
      {obfuscated.transformations && (
        <div className="grid grid-cols-1 md:grid-cols-2 gap-4 pt-6 border-t border-vt-border">
          <StatCard
            label="Transformations Applied"
            value={obfuscated.transformations}
            icon={
              <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M4 4v5h.582m15.356 2A8.001 8.001 0 004.582 9m0 0H9m11 11v-5h-.581m0 0a8.003 8.003 0 01-15.357-2m15.357 2H15" />
              </svg>
            }
          />
          {obfuscated.securityLevel && (
            <StatCard
              label="Security Level"
              value={obfuscated.securityLevel}
              icon={
                <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 15v2m-6 4h12a2 2 0 002-2v-6a2 2 0 00-2-2H6a2 2 0 00-2 2v6a2 2 0 002 2zm10-10V7a4 4 0 00-8 0v4h8z" />
                </svg>
              }
            />
          )}
        </div>
      )}
    </div>
  )
}

