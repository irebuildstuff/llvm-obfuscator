'use client'

import { useState, useRef, useEffect } from 'react'

interface PopoverProps {
  trigger: React.ReactNode
  children: React.ReactNode
  position?: 'bottom-left' | 'bottom-right' | 'top-left' | 'top-right'
  className?: string
}

export default function Popover({ trigger, children, position = 'bottom-right', className = '' }: PopoverProps) {
  const [isOpen, setIsOpen] = useState(false)
  const triggerRef = useRef<HTMLDivElement>(null)
  const popoverRef = useRef<HTMLDivElement>(null)

  useEffect(() => {
    const handleClickOutside = (event: MouseEvent) => {
      if (
        popoverRef.current &&
        triggerRef.current &&
        !popoverRef.current.contains(event.target as Node) &&
        !triggerRef.current.contains(event.target as Node)
      ) {
        setIsOpen(false)
      }
    }

    if (isOpen) {
      document.addEventListener('mousedown', handleClickOutside)
      return () => document.removeEventListener('mousedown', handleClickOutside)
    }
  }, [isOpen])

  useEffect(() => {
    if (isOpen && triggerRef.current && popoverRef.current) {
      const triggerRect = triggerRef.current.getBoundingClientRect()
      const popoverRect = popoverRef.current.getBoundingClientRect()
      
      let top = 0
      let left = 0

      switch (position) {
        case 'bottom-right':
          top = triggerRect.bottom + 8
          left = triggerRect.right - popoverRect.width
          break
        case 'bottom-left':
          top = triggerRect.bottom + 8
          left = triggerRect.left
          break
        case 'top-right':
          top = triggerRect.top - popoverRect.height - 8
          left = triggerRect.right - popoverRect.width
          break
        case 'top-left':
          top = triggerRect.top - popoverRect.height - 8
          left = triggerRect.left
          break
      }

      popoverRef.current.style.top = `${top}px`
      popoverRef.current.style.left = `${left}px`
    }
  }, [isOpen, position])

  return (
    <>
      <div 
        ref={triggerRef}
        onClick={() => setIsOpen(!isOpen)}
        className="cursor-pointer"
      >
        {trigger}
      </div>
      {isOpen && (
        <>
          <div className="fixed inset-0 z-40" onClick={() => setIsOpen(false)} />
          <div
            ref={popoverRef}
            className={`fixed z-50 bg-vt-bg-primary border border-vt-border rounded-lg shadow-xl min-w-[280px] ${className}`}
          >
            {children}
          </div>
        </>
      )}
    </>
  )
}

