'use client'

interface SecurityMeterProps {
  level: 'low' | 'medium' | 'high' | 'maximum'
  value?: number // 0-100
  showLabel?: boolean
  size?: 'sm' | 'md' | 'lg'
}

export default function SecurityMeter({ level, value, showLabel = true, size = 'md' }: SecurityMeterProps) {
  const levelConfig = {
    low: { color: 'bg-vt-warning', text: 'Low', value: 25 },
    medium: { color: 'bg-vt-info', text: 'Medium', value: 50 },
    high: { color: 'bg-vt-primary', text: 'High', value: 75 },
    maximum: { color: 'bg-vt-success', text: 'Maximum', value: 100 }
  }

  const config = levelConfig[level]
  const displayValue = value ?? config.value

  const sizeClasses = {
    sm: 'h-2',
    md: 'h-3',
    lg: 'h-4'
  }

  return (
    <div className="w-full">
      {showLabel && (
        <div className="flex items-center justify-between mb-2">
          <span className="text-sm font-medium text-vt-text-secondary">Security Level</span>
          <span className={`text-sm font-semibold ${
            level === 'low' ? 'text-vt-warning' :
            level === 'medium' ? 'text-vt-info' :
            level === 'high' ? 'text-vt-primary' :
            'text-vt-success'
          }`}>
            {config.text}
          </span>
        </div>
      )}
      <div className={`w-full ${sizeClasses[size]} bg-vt-bg-tertiary rounded-full overflow-hidden`}>
        <div
          className={`h-full ${config.color} rounded-full transition-all duration-700 ease-out`}
          style={{ 
            width: `${displayValue}%`,
            transitionTimingFunction: 'cubic-bezier(0.16, 1, 0.3, 1)'
          }}
        >
          <div className="h-full w-full bg-gradient-to-r from-transparent to-white/20 animate-pulse"></div>
        </div>
      </div>
      {showLabel && (
        <div className="flex justify-between mt-1 text-xs text-vt-text-tertiary">
          <span>Basic</span>
          <span>World-Class</span>
        </div>
      )}
    </div>
  )
}

