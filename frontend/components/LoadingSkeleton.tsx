'use client'

interface LoadingSkeletonProps {
  variant?: 'text' | 'card' | 'circle' | 'rect'
  width?: string | number
  height?: string | number
  className?: string
}

export default function LoadingSkeleton({ 
  variant = 'rect', 
  width, 
  height, 
  className = '' 
}: LoadingSkeletonProps) {
  const baseClasses = 'animate-pulse bg-vt-bg-tertiary rounded'
  
  const variantClasses = {
    text: 'h-4',
    card: 'h-32',
    circle: 'rounded-full',
    rect: ''
  }

  const style: React.CSSProperties = {}
  if (width) style.width = typeof width === 'number' ? `${width}px` : width
  if (height) style.height = typeof height === 'number' ? `${height}px` : height

  return (
    <div
      className={`${baseClasses} ${variantClasses[variant]} ${className}`}
      style={style}
    />
  )
}

// Pre-built skeleton components
export function CardSkeleton() {
  return (
    <div className="border border-vt-border rounded-xl bg-vt-bg-primary p-6">
      <div className="flex items-center gap-4 mb-4">
        <LoadingSkeleton variant="circle" width={48} height={48} />
        <div className="flex-grow">
          <LoadingSkeleton variant="text" width="60%" className="mb-2" />
          <LoadingSkeleton variant="text" width="40%" />
        </div>
      </div>
      <LoadingSkeleton variant="rect" height={100} />
    </div>
  )
}

export function TableSkeleton({ rows = 5 }: { rows?: number }) {
  return (
    <div className="space-y-3">
      {Array.from({ length: rows }).map((_, i) => (
        <div key={i} className="flex items-center gap-4">
          <LoadingSkeleton variant="text" width="30%" />
          <LoadingSkeleton variant="text" width="70%" />
        </div>
      ))}
    </div>
  )
}

