const esbuild = require("esbuild");
const outfile = process.argv[2] || "dist/prose.js";

esbuild.build({
  entryPoints: ["src/index.ts"],
  bundle: true,
  outfile,
  format: "iife",
  globalName: "ProseEditor",
  target: "es2020",
  platform: "browser",
  minify: false,
}).catch(() => process.exit(1));
