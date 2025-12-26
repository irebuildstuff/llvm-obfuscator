'use client'

interface ProgressRingProps {
  progress: number // 0-100
  size?: number
  strokeWidth?: number
  showPercentage?: boolean
  label?: string
}

export default function ProgressRing({ 
  progress, 
  size = 120, 
  strokeWidth = 8,
  showPercentage = true,
  label 
}: ProgressRingProps) {
  const radius = (size - strokeWidth) / 2
  const circumference = radius * 2 * Math.PI
  const offset = circumference - (progress / 100) * circumference

  return (
    <div className="flex flex-col items-center">
      <div className="relative" style={{ width: size, height: size }}>
        <svg
          width={size}
          height={size}
          className="transform -rotate-90"
        >
          {/* Background circle */}
          <circle
            cx={size / 2}
            cy={size / 2}
            r={radius}
            stroke="currentColor"
            strokeWidth={strokeWidth}
            fill="none"
            className="text-vt-bg-tertiary"
          />
          {/* Progress circle */}
          <circle
            cx={size / 2}
            cy={size / 2}
            r={radius}
            stroke="currentColor"
            strokeWidth={strokeWidth}
            fill="none"
            strokeDasharray={circumference}
            strokeDashoffset={offset}
            strokeLinecap="round"
            className="text-vt-primary transition-all duration-700 ease-out"
            style={{
              filter: 'drop-shadow(0 0 6px rgba(99, 102, 241, 0.6))',
              transitionTimingFunction: 'cubic-bezier(0.16, 1, 0.3, 1)'
            }}
          />
        </svg>
        {/* Percentage text */}
        {showPercentage && (
          <div className="absolute inset-0 flex items-center justify-center">
            <div className="text-center">
              <div className="text-2xl font-bold text-vt-text-primary">{Math.round(progress)}%</div>
              {label && (
                <div className="text-xs text-vt-text-tertiary mt-1">{label}</div>
              )}
            </div>
          </div>
        )}
      </div>
    </div>
  )
}

