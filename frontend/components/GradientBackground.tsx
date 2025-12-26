'use client'

interface GradientBackgroundProps {
  className?: string
  variant?: 'primary' | 'subtle' | 'animated'
}

export default function GradientBackground({ 
  className = '', 
  variant = 'subtle' 
}: GradientBackgroundProps) {
  if (variant === 'animated') {
    return (
      <div className={`absolute inset-0 overflow-hidden ${className}`}>
        <div className="absolute inset-0 bg-gradient-to-br from-vt-primary/10 via-transparent to-vt-primary/5 animate-gradient-shift"></div>
        <div className="absolute inset-0 bg-gradient-to-tr from-transparent via-vt-primary/5 to-vt-primary/10 animate-gradient-shift-reverse"></div>
      </div>
    )
  }

  if (variant === 'primary') {
    return (
      <div className={`absolute inset-0 bg-gradient-to-br from-vt-primary/20 via-vt-primary/10 to-transparent ${className}`}></div>
    )
  }

  return (
    <div className={`absolute inset-0 bg-gradient-to-br from-vt-bg-primary via-vt-bg-secondary to-vt-bg-primary ${className}`}></div>
  )
}

