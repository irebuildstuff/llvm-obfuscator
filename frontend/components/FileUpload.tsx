'use client'

import { useState, useCallback, useRef } from 'react'

interface FileUploadProps {
  onFileSelect: (file: File) => void
}

export default function FileUpload({ onFileSelect }: FileUploadProps) {
  const [isDragging, setIsDragging] = useState(false)
  const [selectedFile, setSelectedFile] = useState<File | null>(null)
  const fileInputRef = useRef<HTMLInputElement>(null)
  
  const handleButtonClick = useCallback(() => {
    fileInputRef.current?.click()
  }, [])

  const handleDragOver = useCallback((e: React.DragEvent) => {
    e.preventDefault()
    setIsDragging(true)
  }, [])

  const handleDragLeave = useCallback((e: React.DragEvent) => {
    e.preventDefault()
    setIsDragging(false)
  }, [])

  const handleDrop = useCallback((e: React.DragEvent) => {
    e.preventDefault()
    setIsDragging(false)

    const files = Array.from(e.dataTransfer.files)
    const file = files.find(f => {
      const ext = f.name.toLowerCase().substring(f.name.lastIndexOf('.'))
      return ['.c', '.cpp', '.cc', '.cxx', '.c++'].includes(ext)
    })

    if (file) {
      if (file.size > 10 * 1024 * 1024) {
        alert('File size must be less than 10MB')
        return
      }
      setSelectedFile(file)
      onFileSelect(file)
    } else {
      alert('Please upload a C/C++ source file (.c, .cpp, .cc, .cxx, .c++)')
    }
  }, [onFileSelect])

  const handleFileInput = useCallback((e: React.ChangeEvent<HTMLInputElement>) => {
    const file = e.target.files?.[0]
    if (file) {
      if (file.size > 10 * 1024 * 1024) {
        alert('File size must be less than 10MB')
        return
      }
      setSelectedFile(file)
      onFileSelect(file)
    }
  }, [onFileSelect])

  const getFileTypeDisplay = (filename: string): string => {
    const ext = filename.split('.').pop()?.toLowerCase() || ''
    const typeMap: Record<string, string> = {
      'cpp': 'C++',
      'cxx': 'C++',
      'cc': 'C++',
      'c++': 'C++',
      'c': 'C'
    }
    return typeMap[ext] || ext.toUpperCase()
  }

  return (
    <div className="w-full">
      {!selectedFile ? (
        <>
          {/* Large Premium Upload Area */}
          <div
            onDragOver={handleDragOver}
            onDragLeave={handleDragLeave}
            onDrop={handleDrop}
            className={`
              relative border-2 border-dashed rounded-2xl p-12 sm:p-16 transition-all duration-300 hover-lift
              ${isDragging 
                ? 'border-vt-primary bg-vt-primary-subtle scale-[1.02] shadow-lg glow-on-hover' 
                : 'border-vt-border bg-vt-bg-secondary hover:border-vt-primary/50 hover:bg-vt-bg-primary'
              }
            `}
          >
            {/* Background Pattern */}
            <div className="absolute inset-0 opacity-5">
              <div className="absolute inset-0" style={{
                backgroundImage: `radial-gradient(circle at 2px 2px, var(--vt-primary) 1px, transparent 0)`,
                backgroundSize: '24px 24px'
              }}></div>
            </div>

            <div className="relative z-10 flex flex-col items-center justify-center">
              {/* Large Icon */}
              <div className="mb-6 relative">
                <div className={`
                  w-24 h-24 rounded-2xl flex items-center justify-center transition-all duration-300
                  ${isDragging 
                    ? 'bg-vt-primary text-white scale-110 shadow-xl' 
                    : 'bg-vt-primary-subtle text-vt-primary'
                  }
                `}>
                  <svg
                    className="w-12 h-12"
                    fill="none"
                    stroke="currentColor"
                    viewBox="0 0 24 24"
                  >
                    <path
                      strokeLinecap="round"
                      strokeLinejoin="round"
                      strokeWidth={2}
                      d="M7 16a4 4 0 01-.88-7.903A5 5 0 1115.9 6L16 6a5 5 0 011 9.9M15 13l-3-3m0 0l-3 3m3-3v12"
                    />
                  </svg>
                </div>
                {isDragging && (
                  <div className="absolute -inset-2 bg-vt-primary/20 rounded-2xl animate-pulse"></div>
                )}
              </div>

              {/* Text Content */}
              <h3 className="text-xl font-semibold text-vt-text-primary mb-2">
                {isDragging ? 'Drop your file here' : 'Upload C/C++ Source File'}
              </h3>
              <p className="text-sm text-vt-text-tertiary mb-6 text-center max-w-md">
                Drag and drop your file here, or click to browse
              </p>

              {/* Choose File Button */}
              <div>
                <input
                  ref={fileInputRef}
                  type="file"
                  accept=".c,.cpp,.cc,.cxx,.c++"
                  onChange={handleFileInput}
                  className="hidden"
                />
                <button
                  onClick={handleButtonClick}
                  type="button"
                  className="px-8 py-3.5 bg-vt-primary text-white rounded-lg font-semibold text-base hover:bg-vt-primary-hover transition-all duration-200 shadow-md hover:shadow-lg transform hover:-translate-y-0.5 active:translate-y-0"
                >
                  Choose file
                </button>
              </div>

              {/* File Type Hint */}
              <p className="text-xs text-vt-text-tertiary mt-6">
                Supported formats: .c, .cpp, .cc, .cxx, .c++ (Max 10MB)
              </p>
            </div>
          </div>
        </>
      ) : (
        <div className="border-2 border-vt-border rounded-xl bg-gradient-to-br from-vt-bg-primary to-vt-bg-secondary shadow-lg overflow-hidden">
          <div className="flex items-center gap-5 p-6">
            {/* File Icon - Enhanced */}
            <div className="flex-shrink-0">
              <div className="w-16 h-16 bg-gradient-to-br from-vt-primary to-vt-primary-dark rounded-xl flex items-center justify-center shadow-md border border-vt-primary/20">
                <svg className="w-8 h-8 text-white" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 12h6m-6 4h6m2 5H7a2 2 0 01-2-2V5a2 2 0 012-2h5.586a1 1 0 01.707.293l5.414 5.414a1 1 0 01.293.707V19a2 2 0 01-2 2z" />
                </svg>
              </div>
            </div>
            
            {/* File Info - Enhanced */}
            <div className="flex-grow min-w-0">
              <div className="flex items-center gap-3 mb-2">
                <p className="font-semibold text-lg text-vt-text-primary truncate" title={selectedFile.name}>
                  {selectedFile.name}
                </p>
                <span className="px-2 py-0.5 bg-vt-primary-subtle text-vt-primary text-xs font-medium rounded">
                  {getFileTypeDisplay(selectedFile.name)}
                </span>
              </div>
              <div className="flex items-center gap-6 text-sm text-vt-text-tertiary">
                <span className="flex items-center gap-2">
                  <svg className="w-4 h-4" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M4 7v10c0 2.21 3.582 4 8 4s8-1.79 8-4V7M4 7c0 2.21 3.582 4 8 4s8-1.79 8-4M4 7c0-2.21 3.582-4 8-4s8 1.79 8 4m0 5c0 2.21-3.582 4-8 4s-8-1.79-8-4" />
                  </svg>
                  {selectedFile.size < 1024 
                    ? `${selectedFile.size} B`
                    : selectedFile.size < 1024 * 1024
                    ? `${(selectedFile.size / 1024).toFixed(2)} KB`
                    : `${(selectedFile.size / (1024 * 1024)).toFixed(2)} MB`}
                </span>
                <span className="flex items-center gap-2 text-vt-success">
                  <svg className="w-4 h-4" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                    <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 12l2 2 4-4m6 2a9 9 0 11-18 0 9 9 0 0118 0z" />
                  </svg>
                  Ready to obfuscate
                </span>
              </div>
            </div>
            
            {/* Remove Button - Enhanced */}
            <div className="flex-shrink-0">
              <button
                onClick={() => {
                  setSelectedFile(null)
                  onFileSelect(null as any)
                }}
                className="p-2.5 text-vt-text-tertiary hover:text-vt-danger hover:bg-vt-danger/10 rounded-lg transition-all duration-200 hover:scale-110"
                title="Remove file"
              >
                <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M6 18L18 6M6 6l12 12" />
                </svg>
              </button>
            </div>
          </div>
        </div>
      )}
    </div>
  )
}
