'use client'

import Link from 'next/link'
import Logo from '@/components/Logo'
import ScrollReveal from '@/components/ScrollReveal'

export default function PrivacyPage() {
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
            <h1 className="text-4xl font-bold text-vt-text-primary mb-4">Privacy Policy</h1>
            <p className="text-vt-text-tertiary">Last updated: January 2025</p>
          </div>
        </ScrollReveal>

        <ScrollReveal direction="up" delay={200}>
          <div className="prose prose-lg max-w-none">
            <section className="mb-8">
              <h2 className="text-2xl font-semibold text-vt-text-primary mb-4">1. Introduction</h2>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                LLVM Obfuscator ("we," "our," or "us") is committed to protecting your privacy. This Privacy Policy explains how we collect, use, disclose, and safeguard your information when you use our code obfuscation service.
              </p>
            </section>

            <section className="mb-8">
              <h2 className="text-2xl font-semibold text-vt-text-primary mb-4">2. Information We Collect</h2>
              
              <h3 className="text-xl font-semibold text-vt-text-primary mb-3 mt-6">2.1 Information You Provide</h3>
              <ul className="list-disc list-inside text-vt-text-secondary space-y-2 mb-4">
                <li><strong>Source Code:</strong> C/C++ source files you upload for obfuscation</li>
                <li><strong>Configuration Settings:</strong> Obfuscation presets and custom settings you select</li>
                <li><strong>Contact Information:</strong> If you contact us, we may collect your name, email address, and message content</li>
              </ul>

              <h3 className="text-xl font-semibold text-vt-text-primary mb-3 mt-6">2.2 Automatically Collected Information</h3>
              <ul className="list-disc list-inside text-vt-text-secondary space-y-2 mb-4">
                <li><strong>Usage Data:</strong> Information about how you interact with the Service</li>
                <li><strong>Technical Data:</strong> IP address, browser type, device information, and operating system</li>
                <li><strong>Log Data:</strong> Server logs including timestamps, requests, and error messages</li>
              </ul>
            </section>

            <section className="mb-8">
              <h2 className="text-2xl font-semibold text-vt-text-primary mb-4">3. How We Use Your Information</h2>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                We use the collected information for the following purposes:
              </p>
              <ul className="list-disc list-inside text-vt-text-secondary space-y-2 mb-4">
                <li>To provide, maintain, and improve the obfuscation service</li>
                <li>To process your code obfuscation requests</li>
                <li>To generate obfuscated output and reports</li>
                <li>To respond to your inquiries and provide customer support</li>
                <li>To monitor and analyze usage patterns and trends</li>
                <li>To detect, prevent, and address technical issues or security threats</li>
                <li>To comply with legal obligations</li>
              </ul>
            </section>

            <section className="mb-8">
              <h2 className="text-2xl font-semibold text-vt-text-primary mb-4">4. Data Storage and Retention</h2>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                <strong>Code Storage:</strong> Your uploaded source code and obfuscated output are stored temporarily to process your request. Files are typically retained for a limited period (up to 24 hours) and then automatically deleted.
              </p>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                <strong>Metadata:</strong> Non-sensitive metadata (such as job IDs, timestamps, and configuration settings) may be retained longer for service improvement and analytics purposes.
              </p>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                <strong>Deletion:</strong> You can request deletion of your data at any time by contacting us.
              </p>
            </section>

            <section className="mb-8">
              <h2 className="text-2xl font-semibold text-vt-text-primary mb-4">5. Data Sharing and Disclosure</h2>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                We do not sell, trade, or rent your personal information or source code to third parties. We may share information only in the following circumstances:
              </p>
              <ul className="list-disc list-inside text-vt-text-secondary space-y-2 mb-4">
                <li><strong>Service Providers:</strong> With trusted third-party service providers who assist in operating our Service (e.g., cloud hosting providers), subject to confidentiality agreements</li>
                <li><strong>Legal Requirements:</strong> When required by law, court order, or governmental authority</li>
                <li><strong>Protection of Rights:</strong> To protect our rights, privacy, safety, or property, or that of our users</li>
                <li><strong>Business Transfers:</strong> In connection with a merger, acquisition, or sale of assets, with notice to users</li>
                <li><strong>With Your Consent:</strong> When you explicitly consent to sharing</li>
              </ul>
            </section>

            <section className="mb-8">
              <h2 className="text-2xl font-semibold text-vt-text-primary mb-4">6. Security Measures</h2>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                We implement appropriate technical and organizational security measures to protect your information:
              </p>
              <ul className="list-disc list-inside text-vt-text-secondary space-y-2 mb-4">
                <li>Encryption of data in transit using TLS/SSL</li>
                <li>Secure storage systems with access controls</li>
                <li>Regular security assessments and updates</li>
                <li>Limited access to data on a need-to-know basis</li>
                <li>Automatic deletion of temporary files</li>
              </ul>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                However, no method of transmission over the Internet or electronic storage is 100% secure. While we strive to use commercially acceptable means to protect your information, we cannot guarantee absolute security.
              </p>
            </section>

            <section className="mb-8">
              <h2 className="text-2xl font-semibold text-vt-text-primary mb-4">7. Your Rights and Choices</h2>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                Depending on your location, you may have the following rights regarding your personal information:
              </p>
              <ul className="list-disc list-inside text-vt-text-secondary space-y-2 mb-4">
                <li><strong>Access:</strong> Request access to your personal information</li>
                <li><strong>Correction:</strong> Request correction of inaccurate information</li>
                <li><strong>Deletion:</strong> Request deletion of your personal information</li>
                <li><strong>Portability:</strong> Request transfer of your data</li>
                <li><strong>Objection:</strong> Object to processing of your information</li>
                <li><strong>Restriction:</strong> Request restriction of processing</li>
              </ul>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                To exercise these rights, please contact us using the information provided in the Contact section.
              </p>
            </section>

            <section className="mb-8">
              <h2 className="text-2xl font-semibold text-vt-text-primary mb-4">8. Cookies and Tracking Technologies</h2>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                We may use cookies and similar tracking technologies to track activity on our Service and store certain information. You can instruct your browser to refuse all cookies or to indicate when a cookie is being sent.
              </p>
            </section>

            <section className="mb-8">
              <h2 className="text-2xl font-semibold text-vt-text-primary mb-4">9. Children's Privacy</h2>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                Our Service is not intended for individuals under the age of 13. We do not knowingly collect personal information from children under 13. If you become aware that a child has provided us with personal information, please contact us, and we will take steps to delete such information.
              </p>
            </section>

            <section className="mb-8">
              <h2 className="text-2xl font-semibold text-vt-text-primary mb-4">10. International Data Transfers</h2>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                Your information may be transferred to and maintained on computers located outside of your state, province, country, or other governmental jurisdiction where data protection laws may differ. By using the Service, you consent to the transfer of your information to these facilities.
              </p>
            </section>

            <section className="mb-8">
              <h2 className="text-2xl font-semibold text-vt-text-primary mb-4">11. Changes to This Privacy Policy</h2>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                We may update our Privacy Policy from time to time. We will notify you of any changes by posting the new Privacy Policy on this page and updating the "Last updated" date. You are advised to review this Privacy Policy periodically for any changes.
              </p>
            </section>

            <section className="mb-8">
              <h2 className="text-2xl font-semibold text-vt-text-primary mb-4">12. Contact Us</h2>
              <p className="text-vt-text-secondary leading-relaxed mb-4">
                If you have any questions about this Privacy Policy, please contact us at{' '}
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

