/** @type {import('tailwindcss').Config} */
module.exports = {
  content: [
    './pages/**/*.{js,ts,jsx,tsx,mdx}',
    './components/**/*.{js,ts,jsx,tsx,mdx}',
    './app/**/*.{js,ts,jsx,tsx,mdx}',
  ],
  theme: {
    extend: {
      colors: {
        // Use CSS variables so they respect theme changes
        'vt-primary': 'var(--vt-primary)',
        'vt-bg-primary': 'var(--vt-bg-primary)',
        'vt-bg-secondary': 'var(--vt-bg-secondary)',
        'vt-bg-tertiary': 'var(--vt-bg-tertiary)',
        'vt-bg-subtle': 'var(--vt-bg-subtle)',
        'vt-text-primary': 'var(--vt-text-primary)',
        'vt-text-secondary': 'var(--vt-text-secondary)',
        'vt-text-tertiary': 'var(--vt-text-tertiary)',
        'vt-text-disabled': 'var(--vt-text-disabled)',
        'vt-success': 'var(--vt-success)',
        'vt-warning': 'var(--vt-warning)',
        'vt-danger': 'var(--vt-danger)',
        'vt-info': 'var(--vt-info)',
        'vt-malachite': 'var(--vt-malachite)',
        'vt-critical': 'var(--vt-critical)',
        'vt-border': 'var(--vt-border)',
        'vt-link': 'var(--vt-link-color)',
        'vt-link-hover': 'var(--vt-link-hover-color)',
      },
      fontFamily: {
        sans: ['"Source Sans Pro"', 'Roboto', 'Helvetica', 'Arial', 'sans-serif'],
        mono: ['"Source Code Pro"', 'monospace'],
      },
      maxWidth: {
        'vt-container': '1300px',
      },
      screens: {
        'xs': '475px',
      },
    },
  },
  plugins: [],
}

