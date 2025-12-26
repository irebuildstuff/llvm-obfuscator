'use client'

import { useState, useRef, useEffect } from 'react'

interface TooltipProps {
  children: React.ReactNode
  content: string | React.ReactNode
  position?: 'top' | 'bottom' | 'left' | 'right'
  className?: string
}

export default function Tooltip({ children, content, position = 'bottom', className = '' }: TooltipProps) {
  const [isVisible, setIsVisible] = useState(false)
  const [tooltipPosition, setTooltipPosition] = useState({ top: 0, left: 0 })
  const triggerRef = useRef<HTMLDivElement>(null)
  const tooltipRef = useRef<HTMLDivElement>(null)

  useEffect(() => {
    if (isVisible && triggerRef.current) {
      const triggerRect = triggerRef.current.getBoundingClientRect()
      
      let top = 0
      let left = 0

      switch (position) {
        case 'top':
          top = triggerRect.top - 8
          left = triggerRect.left + triggerRect.width / 2
          break
        case 'bottom':
          top = triggerRect.bottom + 8
          left = triggerRect.left + triggerRect.width / 2
          break
        case 'left':
          top = triggerRect.top + triggerRect.height / 2
          left = triggerRect.left - 8
          break
        case 'right':
          top = triggerRect.top + triggerRect.height / 2
          left = triggerRect.right + 8
          break
      }

      setTooltipPosition({ top, left })
    }
  }, [isVisible, position])

  return (
    <div 
      ref={triggerRef}
      className="relative inline-block"
      onMouseEnter={() => setIsVisible(true)}
      onMouseLeave={() => setIsVisible(false)}
      onClick={() => setIsVisible(!isVisible)}
    >
      {children}
      {isVisible && (
        <div
          ref={tooltipRef}
          className={`fixed z-50 px-3 py-2 bg-vt-text-primary text-white text-xs font-medium rounded-lg shadow-lg whitespace-nowrap pointer-events-none ${className}`}
          style={{
            top: `${tooltipPosition.top}px`,
            left: `${tooltipPosition.left}px`,
            transform: position === 'top' || position === 'bottom' 
              ? 'translate(-50%, 0)' 
              : position === 'left'
              ? 'translate(-100%, -50%)'
              : 'translate(0, -50%)'
          }}
        >
          {content}
          <div 
            className={`absolute w-2 h-2 bg-vt-text-primary transform rotate-45 ${
              position === 'top' ? 'bottom-[-4px] left-1/2 -translate-x-1/2' :
              position === 'bottom' ? 'top-[-4px] left-1/2 -translate-x-1/2' :
              position === 'left' ? 'right-[-4px] top-1/2 -translate-y-1/2' :
              'left-[-4px] top-1/2 -translate-y-1/2'
            }`}
          />
        </div>
      )}
    </div>
  )
}

