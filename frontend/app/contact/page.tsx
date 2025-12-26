'use client'

import { useState } from 'react'
import Link from 'next/link'
import Logo from '@/components/Logo'
import ScrollReveal from '@/components/ScrollReveal'

export default function ContactPage() {
  const [formData, setFormData] = useState({
    name: '',
    email: '',
    subject: '',
    message: ''
  })
  const [isSubmitting, setIsSubmitting] = useState(false)
  const [submitStatus, setSubmitStatus] = useState<'idle' | 'success' | 'error'>('idle')

  const handleChange = (e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement | HTMLSelectElement>) => {
    setFormData({
      ...formData,
      [e.target.name]: e.target.value
    })
  }

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault()
    setIsSubmitting(true)
    setSubmitStatus('idle')

    // Simulate form submission
    setTimeout(() => {
      setIsSubmitting(false)
      setSubmitStatus('success')
      setFormData({ name: '', email: '', subject: '', message: '' })
      
      // Reset success message after 5 seconds
      setTimeout(() => {
        setSubmitStatus('idle')
      }, 5000)
    }, 1000)
  }

  return (
    <main className="min-h-screen bg-vt-bg-primary">
      {/* Header */}
      <header className="sticky top-0 z-50 border-b border-vt-border bg-vt-bg-primary/95 backdrop-blur-sm shadow-sm">
        <div className="vt-container py-4">
          <div className="flex items-center justify-between">
            <Logo size="md" showText={true} />
            <nav className="flex items-center gap-4">
              <Link href="/" className="text-sm font-medium text-vt-text-secondary hover:text-vt-primary transition-colors">
                Home
              </Link>
              <Link href="/presets" className="text-sm font-medium text-vt-text-secondary hover:text-vt-primary transition-colors">
                Presets
              </Link>
            </nav>
          </div>
        </div>
      </header>

      <div className="vt-container py-12 max-w-3xl mx-auto">
        <ScrollReveal direction="up" delay={100}>
          <div className="mb-8">
            <Link href="/" className="text-vt-primary hover:underline mb-4 inline-block text-sm">
              ← Back to Home
            </Link>
            <h1 className="text-4xl font-bold text-vt-text-primary mb-4">Contact Us</h1>
            <p className="text-vt-text-secondary text-lg">
              Have questions, feedback, or need support? We'd love to hear from you.
            </p>
          </div>
        </ScrollReveal>

        <div className="grid grid-cols-1 md:grid-cols-2 gap-8 mb-12">
          <ScrollReveal direction="up" delay={200}>
            <div className="space-y-6">
              <div>
                <h2 className="text-2xl font-semibold text-vt-text-primary mb-4">Get in Touch</h2>
                <p className="text-vt-text-secondary leading-relaxed mb-6">
                  Whether you have questions about our obfuscation service, need technical support, or want to provide feedback, we're here to help.
                </p>
              </div>

              <div className="space-y-4">
                <div className="flex items-start gap-4">
                  <div className="w-10 h-10 bg-vt-primary-subtle rounded-lg flex items-center justify-center flex-shrink-0">
                    <svg className="w-5 h-5 text-vt-primary" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M3 8l7.89 5.26a2 2 0 002.22 0L21 8M5 19h14a2 2 0 002-2V7a2 2 0 00-2-2H5a2 2 0 00-2 2v10a2 2 0 002 2z" />
                    </svg>
                  </div>
                  <div>
                    <h3 className="font-semibold text-vt-text-primary mb-1">Email</h3>
                    <p className="text-vt-text-secondary text-sm">support@llvmobfuscator.com</p>
                  </div>
                </div>

                <div className="flex items-start gap-4">
                  <div className="w-10 h-10 bg-vt-primary-subtle rounded-lg flex items-center justify-center flex-shrink-0">
                    <svg className="w-5 h-5 text-vt-primary" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 8v4l3 3m6-3a9 9 0 11-18 0 9 9 0 0118 0z" />
                    </svg>
                  </div>
                  <div>
                    <h3 className="font-semibold text-vt-text-primary mb-1">Response Time</h3>
                    <p className="text-vt-text-secondary text-sm">We typically respond within 24-48 hours</p>
                  </div>
                </div>

                <div className="flex items-start gap-4">
                  <div className="w-10 h-10 bg-vt-primary-subtle rounded-lg flex items-center justify-center flex-shrink-0">
                    <svg className="w-5 h-5 text-vt-primary" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                      <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M8.228 9c.549-1.165 2.03-2 3.772-2 2.21 0 4 1.343 4 3 0 1.4-1.278 2.575-3.006 2.907-.542.104-.994.54-.994 1.093m0 3h.01M21 12a9 9 0 11-18 0 9 9 0 0118 0z" />
                    </svg>
                  </div>
                  <div>
                    <h3 className="font-semibold text-vt-text-primary mb-1">Support</h3>
                    <p className="text-vt-text-secondary text-sm">Technical support and general inquiries</p>
                  </div>
                </div>
              </div>
            </div>
          </ScrollReveal>

          <ScrollReveal direction="up" delay={300}>
            <div className="border border-vt-border rounded-xl bg-vt-bg-secondary p-6">
              <h2 className="text-xl font-semibold text-vt-text-primary mb-6">Send us a Message</h2>
              
              <form onSubmit={handleSubmit} className="space-y-4">
                <div>
                  <label htmlFor="name" className="block text-sm font-medium text-vt-text-secondary mb-2">
                    Name *
                  </label>
                  <input
                    type="text"
                    id="name"
                    name="name"
                    required
                    value={formData.name}
                    onChange={handleChange}
                    className="w-full px-4 py-2.5 border border-vt-border rounded-lg bg-vt-bg-primary text-vt-text-primary focus:outline-none focus:ring-2 focus:ring-vt-primary focus:border-transparent transition-all"
                    placeholder="Your name"
                  />
                </div>

                <div>
                  <label htmlFor="email" className="block text-sm font-medium text-vt-text-secondary mb-2">
                    Email *
                  </label>
                  <input
                    type="email"
                    id="email"
                    name="email"
                    required
                    value={formData.email}
                    onChange={handleChange}
                    className="w-full px-4 py-2.5 border border-vt-border rounded-lg bg-vt-bg-primary text-vt-text-primary focus:outline-none focus:ring-2 focus:ring-vt-primary focus:border-transparent transition-all"
                    placeholder="your.email@example.com"
                  />
                </div>

                <div>
                  <label htmlFor="subject" className="block text-sm font-medium text-vt-text-secondary mb-2">
                    Subject *
                  </label>
                  <select
                    id="subject"
                    name="subject"
                    required
                    value={formData.subject}
                    onChange={handleChange}
                    className="w-full px-4 py-2.5 border border-vt-border rounded-lg bg-vt-bg-primary text-vt-text-primary focus:outline-none focus:ring-2 focus:ring-vt-primary focus:border-transparent transition-all"
                  >
                    <option value="">Select a subject</option>
                    <option value="support">Technical Support</option>
                    <option value="feature">Feature Request</option>
                    <option value="bug">Bug Report</option>
                    <option value="general">General Inquiry</option>
                    <option value="partnership">Partnership</option>
                  </select>
                </div>

                <div>
                  <label htmlFor="message" className="block text-sm font-medium text-vt-text-secondary mb-2">
                    Message *
                  </label>
                  <textarea
                    id="message"
                    name="message"
                    required
                    rows={6}
                    value={formData.message}
                    onChange={handleChange}
                    className="w-full px-4 py-2.5 border border-vt-border rounded-lg bg-vt-bg-primary text-vt-text-primary focus:outline-none focus:ring-2 focus:ring-vt-primary focus:border-transparent transition-all resize-none"
                    placeholder="Tell us how we can help..."
                  />
                </div>

                {submitStatus === 'success' && (
                  <div className="p-4 bg-vt-success/10 border border-vt-success/30 rounded-lg">
                    <p className="text-sm text-vt-success font-medium">
                      ✓ Thank you! Your message has been sent. We'll get back to you soon.
                    </p>
                  </div>
                )}

                {submitStatus === 'error' && (
                  <div className="p-4 bg-vt-danger/10 border border-vt-danger/30 rounded-lg">
                    <p className="text-sm text-vt-danger font-medium">
                      ✗ Something went wrong. Please try again later.
                    </p>
                  </div>
                )}

                <button
                  type="submit"
                  disabled={isSubmitting}
                  className="w-full px-6 py-3 bg-vt-primary text-white rounded-lg font-semibold hover:bg-vt-primary-hover transition-all duration-200 shadow-md hover:shadow-lg hover-lift disabled:opacity-50 disabled:cursor-not-allowed disabled:hover:transform-none"
                >
                  {isSubmitting ? 'Sending...' : 'Send Message'}
                </button>
              </form>
            </div>
          </ScrollReveal>
        </div>

        <ScrollReveal direction="up" delay={400}>
          <div className="border-t border-vt-border pt-8">
            <h2 className="text-xl font-semibold text-vt-text-primary mb-4">Frequently Asked Questions</h2>
            <div className="space-y-4">
              <div>
                <h3 className="font-semibold text-vt-text-primary mb-2">How do I report a bug?</h3>
                <p className="text-vt-text-secondary text-sm">
                  Use the contact form above and select "Bug Report" as the subject. Please include details about the issue, steps to reproduce, and any error messages.
                </p>
              </div>
              <div>
                <h3 className="font-semibold text-vt-text-primary mb-2">Can I request a new feature?</h3>
                <p className="text-vt-text-secondary text-sm">
                  Absolutely! Select "Feature Request" in the contact form and describe the feature you'd like to see. We review all suggestions.
                </p>
              </div>
              <div>
                <h3 className="font-semibold text-vt-text-primary mb-2">Is my code kept private?</h3>
                <p className="text-vt-text-secondary text-sm">
                  Yes. Your source code is processed temporarily and automatically deleted after 24 hours. See our{' '}
                  <Link href="/privacy" className="text-vt-primary hover:underline">Privacy Policy</Link> for more details.
                </p>
              </div>
            </div>
          </div>
        </ScrollReveal>
      </div>

      {/* Footer */}
      <footer className="border-t border-vt-border bg-vt-bg-secondary mt-24 py-12">
        <div className="vt-container text-center text-sm text-vt-text-tertiary">
          <p>© 2025 LLVM Obfuscator. All rights reserved.</p>
        </div>
      </footer>
    </main>
  )
}

