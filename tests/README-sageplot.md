# Optional SagePlot integration regression check

Build tests/sageplot-optional.tex using Ximera's normal PDF and HTML workflows.

Expected behavior:

1. No SageTeX installed: the class provides a harmless SagePlot command accepting zero, one, or two optional arguments. Compile succeeds; no PDF plot is rendered.
2. SageTeX installed: normal PDF SagePlot behavior remains unchanged; actual PDF image production still requires a SageTeX/Sage processing step as before.
3. HTML output: three passive span.sage-plot markers, each containing a script[type="text/x-sage-plot"] element with its authored expression as text. The two optional presentation arguments do not appear in the marker.
4. HTML on a server WITHOUT SagePlot rendering: scripts use a nonexecuting MIME type, so no Sage execution, network call, or Xronos-specific JavaScript is requested by this feature. Empty holders should have no disruptive layout or script errors.
5. On a compatible Xronos server: the existing SagePlot renderer detects each marker and replaces it with its plot SVG. Test the existing xronosuf/testFiles:testSuite/14-sageplot-svg.tex fixture separately for seeded/randomized graph and answer-key synchronization.

This is a feature of XimeraLaTeX only. It does not add or depend on server changes to XimeraProject/server. Inert markers will not display plots on unsupported servers.

Note: Compiler warnings/errors caused by other Sage macros on an unsupported installation are out of scope. This fixture uses SagePlot only.
