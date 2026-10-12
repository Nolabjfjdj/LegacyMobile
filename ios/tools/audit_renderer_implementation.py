import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CLIENT = ROOT / "Minecraft.Client"
HEADER = CLIENT / "Windows64" / "4JLibs" / "inc" / "4J_Render.h"
METHOD_RE = re.compile(r"^\s*(?:virtual\s+)?[\w:*&<>\s]+?\s+(\w+)\s*\([^;{}]*\)\s*(?:const\s*)?;\s*(?://.*)?$")
DEFINITION_RE = re.compile(r"\bC4JRender\s*::\s*(\w+)\s*\(")
EXCLUDED_PARTS = {"Windows64", "Durango", "Orbis", "PS3", "PSVita", "Xbox", "Xbox360"}


def read_text(path):
    return path.read_text(encoding="utf-8-sig", errors="replace")


def main():
    if not HEADER.is_file():
        raise SystemExit(f"Renderer API header missing: {HEADER.relative_to(ROOT)}")

    header = read_text(HEADER)
    in_class = False
    brace_depth = 0
    declarations = set()
    for line in header.splitlines():
        if re.search(r"\bclass\s+C4JRender\b", line):
            in_class = True
        if in_class:
            brace_depth += line.count("{") - line.count("}")
            match = METHOD_RE.match(line)
            if match and match.group(1) not in {"if", "while", "switch", "return"}:
                declarations.add(match.group(1))
            if brace_depth == 0 and "}" in line:
                in_class = False

    definitions = {}
    platform_definitions = {}
    source_files = []
    platform_source_files = []
    for path in CLIENT.rglob("*"):
        if not path.is_file() or path.suffix.lower() not in {".cpp", ".cc", ".c", ".mm"}:
            continue
        is_platform_specific = any(part in EXCLUDED_PARTS for part in path.relative_to(CLIENT).parts)
        target = platform_definitions if is_platform_specific else definitions
        if is_platform_specific:
            platform_source_files.append(path)
        else:
            source_files.append(path)
        for name in DEFINITION_RE.findall(read_text(path)):
            target.setdefault(name, []).append(path.relative_to(ROOT).as_posix())

    libs = sorted(
        path.relative_to(ROOT).as_posix()
        for path in CLIENT.rglob("*")
        if path.is_file() and path.suffix.lower() in {".lib", ".a", ".dylib", ".framework"}
    )
    implemented = sorted(declarations & definitions.keys())
    platform_implemented = sorted(declarations & platform_definitions.keys())
    missing = sorted(declarations - definitions.keys())
    report = {
        "report_type": "renderer_implementation_surface_audit_not_build_validation",
        "renderer_header": HEADER.relative_to(ROOT).as_posix(),
        "declared_method_count": len(declarations),
        "source_files_scanned": len(source_files),
        "platform_specific_source_files_scanned": len(platform_source_files),
        "methods_with_portable_source_definitions": len(implemented),
        "methods_with_platform_specific_source_definitions": len(platform_implemented),
        "methods_without_visible_source_definitions": len(missing),
        "portable_source_definitions": {
            name: sorted(definitions[name]) for name in implemented
        },
        "platform_specific_source_definitions": {
            name: sorted(platform_definitions[name]) for name in platform_implemented
        },
        "methods_without_visible_source_definitions_list": missing,
        "prebuilt_renderer_or_platform_libraries": libs,
        "limitations": [
            "Text-based method matching is heuristic and does not parse C++ overloads or macros.",
            "Definitions inside excluded platform-specific directories are not treated as an iOS backend.",
            "A method absent from source may exist in a prebuilt binary library.",
            "This is not a compiler or linker test."
        ]
    }
    output = Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / "ios" / "renderer-implementation-audit.json"
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(f"Renderer methods declared: {len(declarations)}")
    print(f"Methods with portable source definitions: {len(implemented)}")
    print(f"Methods with platform-specific source definitions: {len(platform_implemented)}")
    print(f"Methods without visible source definitions: {len(missing)}")
    print(f"Prebuilt/platform libraries found: {len(libs)}")
    print(f"Report written to: {output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
