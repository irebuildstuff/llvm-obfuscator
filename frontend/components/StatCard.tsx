'use client'

interface StatCardProps {
  label: string
  value: string | number
  icon?: React.ReactNode
  trend?: 'up' | 'down' | 'neutral'
  description?: string
  className?: string
}

export default function StatCard({ label, value, icon, trend, description, className = '' }: StatCardProps) {
  return (
    <div className={`border border-vt-border rounded-xl bg-vt-bg-primary p-6 hover:shadow-md transition-all duration-200 hover-lift ${className}`}>
      <div className="flex items-start justify-between mb-4">
        <div className="flex items-center gap-3">
          {icon && (
            <div className="w-10 h-10 bg-vt-primary-subtle rounded-lg flex items-center justify-center text-vt-primary">
              {icon}
            </div>
          )}
          <div>
            <p className="text-sm font-medium text-vt-text-tertiary mb-1">{label}</p>
            <p className="text-2xl font-bold text-vt-text-primary">{value}</p>
          </div>
        </div>
        {trend && (
          <div className={`flex items-center gap-1 text-xs font-medium ${
            trend === 'up' ? 'text-vt-success' :
            trend === 'down' ? 'text-vt-danger' :
            'text-vt-text-tertiary'
          }`}>
            {trend === 'up' && (
              <svg className="w-4 h-4" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M13 7l5 5m0 0l-5 5m5-5H6" />
              </svg>
            )}
            {trend === 'down' && (
              <svg className="w-4 h-4" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M13 17l5-5m0 0l-5-5m5 5H6" />
              </svg>
            )}
          </div>
        )}
      </div>
      {description && (
        <p className="text-xs text-vt-text-tertiary">{description}</p>
      )}
    </div>
  )
}

