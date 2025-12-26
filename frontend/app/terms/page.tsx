'use client'

import Link from 'next/link'
import Logo from '@/components/Logo'
import ScrollReveal from '@/components/ScrollReveal'

export default function TermsPage() {
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

      <div className="vt-container py-12 max-w-4xl mx-auto">
        <ScrollReveal direction="up" delay={100}>
          <div className="mb-8">
            <Link href="/" className="text-vt-primary hover:underline mb-4 inline-block text-sm">
              ← Back to Home
            </Link>
            <h1 className="text-4xl font-bold text-vt-text-primary mb-4">Terms of Service</h1>
            <p className="text-vt-text-tertiary">Last updated: January 2025</p>
          </div>
        </ScrollReveal>

        <ScrollReveal direction="up" delay={200}>
          <div className="prose prose-lg max-w-none">
            <section className="mb-8">
              <h2 className="text-2xl font-semibold text-vt-text-primary mb-4">1. Acceptance of Terms</h2>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                By accessing and using LLVM Obfuscator ("the Service"), you accept and agree to be bound by the terms and provision of this agreement. If you do not agree to abide by the above, please do not use this service.
              </p>
            </section>

            <section className="mb-8">
              <h2 className="text-2xl font-semibold text-vt-text-primary mb-4">2. Description of Service</h2>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                LLVM Obfuscator is a code obfuscation tool that transforms C/C++ source code into obfuscated executables. The Service provides:
              </p>
              <ul className="list-disc list-inside text-vt-text-secondary space-y-2 mb-4">
                <li>Code obfuscation using advanced techniques</li>
                <li>Generation of obfuscated LLVM IR and executables</li>
                <li>Detailed obfuscation reports</li>
                <li>Multiple preset configurations</li>
              </ul>
            </section>

            <section className="mb-8">
              <h2 className="text-2xl font-semibold text-vt-text-primary mb-4">3. User Responsibilities</h2>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                You agree to:
              </p>
              <ul className="list-disc list-inside text-vt-text-secondary space-y-2 mb-4">
                <li>Use the Service only for lawful purposes</li>
                <li>Not upload malicious code, viruses, or harmful software</li>
                <li>Not attempt to reverse engineer or compromise the Service</li>
                <li>Not use the Service to obfuscate code for illegal activities</li>
                <li>Maintain the confidentiality of any credentials provided</li>
              </ul>
            </section>

            <section className="mb-8">
              <h2 className="text-2xl font-semibold text-vt-text-primary mb-4">4. Intellectual Property</h2>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                The Service and its original content, features, and functionality are owned by LLVM Obfuscator and are protected by international copyright, trademark, patent, trade secret, and other intellectual property laws.
              </p>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                You retain all rights to code you upload and obfuscate. The Service does not claim ownership of your source code or obfuscated output.
              </p>
            </section>

            <section className="mb-8">
              <h2 className="text-2xl font-semibold text-vt-text-primary mb-4">5. Code Submission and Processing</h2>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                When you submit code for obfuscation:
              </p>
              <ul className="list-disc list-inside text-vt-text-secondary space-y-2 mb-4">
                <li>Your code is processed temporarily and may be stored for a limited time</li>
                <li>We do not share your code with third parties without your consent</li>
                <li>You are responsible for ensuring you have the right to obfuscate the submitted code</li>
                <li>We reserve the right to refuse processing of code that violates these terms</li>
              </ul>
            </section>

            <section className="mb-8">
              <h2 className="text-2xl font-semibold text-vt-text-primary mb-4">6. Disclaimer of Warranties</h2>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                THE SERVICE IS PROVIDED "AS IS" AND "AS AVAILABLE" WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
              </p>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                We do not warrant that the Service will be uninterrupted, secure, or error-free, or that defects will be corrected.
              </p>
            </section>

            <section className="mb-8">
              <h2 className="text-2xl font-semibold text-vt-text-primary mb-4">7. Limitation of Liability</h2>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                IN NO EVENT SHALL LLVM OBFUSCATOR, ITS AFFILIATES, OR THEIR RESPECTIVE OFFICERS, DIRECTORS, EMPLOYEES, OR AGENTS BE LIABLE FOR ANY INDIRECT, INCIDENTAL, SPECIAL, CONSEQUENTIAL, OR PUNITIVE DAMAGES, INCLUDING WITHOUT LIMITATION, LOSS OF PROFITS, DATA, USE, GOODWILL, OR OTHER INTANGIBLE LOSSES, RESULTING FROM YOUR USE OF THE SERVICE.
              </p>
            </section>

            <section className="mb-8">
              <h2 className="text-2xl font-semibold text-vt-text-primary mb-4">8. Indemnification</h2>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                You agree to defend, indemnify, and hold harmless LLVM Obfuscator and its affiliates from and against any claims, liabilities, damages, losses, and expenses, including without limitation reasonable legal and accounting fees, arising out of or in any way connected with your access to or use of the Service or your violation of these Terms.
              </p>
            </section>

            <section className="mb-8">
              <h2 className="text-2xl font-semibold text-vt-text-primary mb-4">9. Modifications to Terms</h2>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                We reserve the right to modify these Terms at any time. We will notify users of any material changes by updating the "Last updated" date at the top of this page. Your continued use of the Service after such modifications constitutes acceptance of the updated Terms.
              </p>
            </section>

            <section className="mb-8">
              <h2 className="text-2xl font-semibold text-vt-text-primary mb-4">10. Termination</h2>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                We may terminate or suspend your access to the Service immediately, without prior notice or liability, for any reason whatsoever, including without limitation if you breach the Terms.
              </p>
            </section>

            <section className="mb-8">
              <h2 className="text-2xl font-semibold text-vt-text-primary mb-4">11. Governing Law</h2>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                These Terms shall be governed and construed in accordance with applicable laws, without regard to its conflict of law provisions.
              </p>
            </section>

            <section className="mb-8">
              <h2 className="text-2xl font-semibold text-vt-text-primary mb-4">12. Contact Information</h2>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                If you have any questions about these Terms of Service, please contact us at{' '}
                <Link href="/contact" className="text-vt-primary hover:underline">our contact page</Link>.
              </p>
            </section>
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

