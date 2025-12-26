'use client'

import { useEffect, useRef, useState } from 'react'

interface ScrollRevealProps {
  children: React.ReactNode
  delay?: number
  direction?: 'up' | 'down' | 'left' | 'right' | 'fade'
  duration?: number
  className?: string
}

export default function ScrollReveal({ 
  children, 
  delay = 0, 
  direction = 'up',
  duration = 600,
  className = '' 
}: ScrollRevealProps) {
  const [isVisible, setIsVisible] = useState(false)
  const ref = useRef<HTMLDivElement>(null)

  useEffect(() => {
    const observer = new IntersectionObserver(
      ([entry]) => {
        if (entry.isIntersecting) {
          setTimeout(() => {
            setIsVisible(true)
          }, delay)
        }
      },
      {
        threshold: 0.1,
        rootMargin: '0px 0px -50px 0px'
      }
    )

    if (ref.current) {
      observer.observe(ref.current)
    }

    return () => {
      if (ref.current) {
        observer.unobserve(ref.current)
      }
    }
  }, [delay])

  const directionClasses = {
    up: isVisible ? 'translate-y-0 opacity-100' : 'translate-y-8 opacity-0',
    down: isVisible ? 'translate-y-0 opacity-100' : '-translate-y-8 opacity-0',
    left: isVisible ? 'translate-x-0 opacity-100' : 'translate-x-8 opacity-0',
    right: isVisible ? 'translate-x-0 opacity-100' : '-translate-x-8 opacity-0',
    fade: isVisible ? 'opacity-100' : 'opacity-0'
  }

  return (
    <div
      ref={ref}
      className={`transition-all ease-out ${directionClasses[direction]} ${className}`}
      style={{
        transitionDuration: `${duration}ms`,
        transitionTimingFunction: 'cubic-bezier(0.16, 1, 0.3, 1)'
      }}
    >
      {children}
    </div>
  )
}

