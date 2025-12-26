'use client'

import Link from 'next/link'

interface LogoProps {
  size?: 'sm' | 'md' | 'lg' | 'xl'
  showText?: boolean
  className?: string
}

export default function Logo({ size = 'md', showText = true, className = '' }: LogoProps) {
  const sizeClasses = {
    sm: 'w-6 h-6 text-lg',
    md: 'w-8 h-8 text-xl',
    lg: 'w-12 h-12 text-3xl',
    xl: 'w-16 h-16 text-4xl'
  }

  const textSizeClasses = {
    sm: 'text-base',
    md: 'text-lg',
    lg: 'text-2xl',
    xl: 'text-3xl'
  }

  return (
    <Link href="/" className={`flex items-center gap-2.5 group ${className}`}>
      {/* Professional Logo Icon - Shield with Code Symbol */}
      <div className={`relative ${sizeClasses[size]}`}>
        {/* Shield Background */}
        <div className="absolute inset-0 bg-gradient-to-br from-vt-primary via-vt-primary to-vt-primary-dark rounded-lg shadow-lg transform group-hover:scale-110 transition-all duration-300 ease-out">
          <div className="absolute inset-0 bg-gradient-to-tr from-white/20 to-transparent rounded-lg"></div>
        </div>
        
        {/* Code Symbol Overlay */}
        <div className="relative flex items-center justify-center h-full text-white font-bold">
          <svg 
            viewBox="0 0 24 24" 
            fill="none" 
            stroke="currentColor" 
            strokeWidth="2.5"
            className="w-full h-full p-1.5"
          >
            {/* Shield shape */}
            <path d="M12 2L4 5v6c0 5.55 3.84 10.74 8 12 4.16-1.26 8-6.45 8-12V5l-8-3z" />
            {/* Code brackets */}
            <path d="M8 8l-2 2 2 2M16 8l2 2-2 2" strokeLinecap="round" />
            {/* Center line */}
            <path d="M10 12h4" strokeLinecap="round" />
          </svg>
        </div>
        
        {/* Subtle glow effect */}
        <div className="absolute inset-0 rounded-lg bg-vt-primary/20 blur-md opacity-0 group-hover:opacity-100 transition-opacity duration-300 -z-10"></div>
      </div>
      
      {/* Logo Text */}
      {showText && (
        <div className="flex flex-col">
          <span className={`font-bold text-vt-text-primary tracking-tight ${textSizeClasses[size]} group-hover:text-vt-primary transition-colors duration-200`}>
            LLVM Obfuscator
          </span>
          <span className={`text-xs text-vt-text-tertiary -mt-0.5 ${size === 'xl' ? 'text-sm' : ''}`}>
            World-Class Protection
          </span>
        </div>
      )}
    </Link>
  )
}

