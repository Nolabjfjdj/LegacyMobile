import json
import re
import sys
from pathlib import Path
from xml.etree import ElementTree as ET

ROOT = Path(__file__).resolve().parents[2]
PROJECTS = [
    ROOT / "Minecraft.Client" / "Minecraft.Client.vcxproj",
    ROOT / "Minecraft.World" / "Minecraft.World.vcxproj",
]
INCLUDE_RE = re.compile(r'^\s*#\s*include\s*[<"]([^">]+)[">]')
PLATFORM_RE = re.compile(
    r"(?:^|[/\\])(?:Windows64|Durango|Orbis|PS3|PSVita|Xbox360|"
    r"Windows|Win32|SPU_Tasks)(?:[/\\]|$)|"
    r"(?:windows\.h|d3d\w*\.h|dxgi\.h|DirectXMath\.h|"
    r"xdk\.h|xuiapp\.h|scebase\.h|Ps3Types\.h|OrbisTypes\.h|"
    r"PSVitaTypes\.h|pthread_np\.h)",
    re.IGNORECASE,
)
NS = {"msbuild": "http://schemas.microsoft.com/developer/msbuild/2003"}
SOURCE_SUFFIXES = {".h", ".hpp", ".hh", ".c", ".cc", ".cpp", ".mm"}


def read_text(path):
    return path.read_text(encoding="utf-8-sig", errors="replace")


def project_sources():
    found = set()
    errors = []
    for project in PROJECTS:
        if not project.is_file():
            errors.append(f"Missing project: {project.relative_to(ROOT)}")
            continue
        try:
            node = ET.parse(project).getroot()
        except ET.ParseError as exc:
            errors.append(f"Invalid project XML {project.relative_to(ROOT)}: {exc}")
            continue
        for item in node.findall(".//msbuild:ClCompile", NS):
            name = item.attrib.get("Include")
            if name:
                found.add((project.parent / name.replace("\\", "/")).resolve())
    return found, errors


def resolve_include(including_file, include):
    include = include.replace("\\\\", "/")
    candidate = (including_file.parent / include).resolve()
    if candidate.is_file():
        return candidate
    candidate = (ROOT / include).resolve()
    if candidate.is_file():
        return candidate
    return None


def main():
    sources, errors = project_sources()
    graph = {}
    direct_platform = {}
    missing_local = {}
    visited_files = set()

    for source in sorted(sources):
        if not source.is_file():
            errors.append(f"Listed source missing: {source}")
            continue
        queue = [source]
        visited = set()
        deps = set()
        platforms = set()
        missing = set()
        while queue:
            current = queue.pop()
            if current in visited or not current.is_file():
                continue
            visited.add(current)
            visited_files.add(current)
            relative_current = current.relative_to(ROOT).as_posix() if current.is_relative_to(ROOT) else str(current)
            if PLATFORM_RE.search(relative_current):
                platforms.add(relative_current)
            try:
                lines = read_text(current).splitlines()
            except OSError:
                continue
            for line_no, line in enumerate(lines, 1):
                match = INCLUDE_RE.match(line)
                if not match:
                    continue
                inc = match.group(1)
                if PLATFORM_RE.search(inc):
                    platforms.add(f"{relative_current}:{line_no}: {inc}")
                if inc.startswith(("Windows.h", "windows.h", "d3d", "dxgi", "DirectX", "xdk", "xuiapp", "scebase", "Ps3Types", "OrbisTypes", "PSVitaTypes")):
                    platforms.add(f"{relative_current}:{line_no}: {inc}")
                if inc.startswith(("<", "boost/")):
                    continue
                resolved = resolve_include(current, inc)
                if resolved:
                    if resolved not in deps:
                        deps.add(resolved)
                        queue.append(resolved)
                elif inc not in {
                    "stdio.h", "stdlib.h", "string", "string.h", "vector", "map", "set",
                    "list", "memory", "algorithm", "math.h", "cmath", "stdint.h", "cstdint",
                    "assert.h", "assert", "time.h", "stddef.h", "stdarg.h", "limits.h",
                    "float.h", "ctype.h", "errno.h", "new", "typeinfo", "exception",
                    "functional", "utility", "array", "tuple", "initializer_list", "iostream",
                    "fstream", "sstream", "bitset", "climits", "cfloat", "cstring", "cstdlib",
                    "cstdio", "cstdarg", "cwchar", "wchar.h", "unistd.h", "pthread.h",
                    "unordered_map", "unordered_set", "deque", "queue", "stack", "limits",
                    "map", "set", "list", "iterator", "numeric", "random", "chrono", "thread",
                    "mutex", "condition_variable", "atomic", "optional", "variant", "string_view",
                    "regex", "locale", "codecvt", "filesystem", "ios", "iosfwd", "iomanip",
                    "fstream", "sstream", "streambuf", "cstdio", "cstdlib", "cstring", "cctype",
                    "cwctype", "cerrno", "clocale", "csignal", "csetjmp", "cstdarg", "cstddef",
                    "cstdint", "cmath", "cfenv", "cinttypes", "cfloat", "climits", "ctime",
                    "cuchar", "cwchar", "cwctype", "type_traits", "typeindex", "typeinfo",
                    "memory_resource", "scoped_allocator", "forward_list", "unordered_map",
                    "unordered_set", "shared_mutex", "future", "system_error", "stdexcept",
                    "cassert", "ciso646", "ccomplex", "ctgmath", "cstdbool", "cstdalign",
                    "cstdnoreturn", "barrier", "latch", "semaphore", "span", "ranges"
                }:
                    missing.add(f"{relative_current}:{line_no}: {inc}")
        graph[source.relative_to(ROOT).as_posix()] = sorted(
            p.relative_to(ROOT).as_posix() if p.is_relative_to(ROOT) else str(p) for p in deps
        )
        direct_platform[source.relative_to(ROOT).as_posix()] = sorted(platforms)
        missing_local[source.relative_to(ROOT).as_posix()] = sorted(missing)

    candidates = [
        name for name in sorted(graph)
        if not direct_platform.get(name) and not missing_local.get(name)
    ]
    report = {
        "report_type": "transitive_include_probe_not_compiler_validation",
        "project_source_count": len(sources),
        "visited_file_count": len(visited_files),
        "source_files_with_no_detected_platform_or_unresolved_includes": len(candidates),
        "candidate_source_files": candidates,
        "include_graph": graph,
        "platform_include_hits_by_source": {k: v for k, v in direct_platform.items() if v},
        "unresolved_nonstandard_includes_by_source": {k: v for k, v in missing_local.items() if v},
        "errors": errors,
        "limitations": [
            "This is a conservative text scan, not an Apple Clang compile.",
            "Preprocessor conditions are not evaluated, so inactive platform includes can be reported.",
            "System and third-party include classification is heuristic.",
            "A source file can still fail to compile because of types, APIs, linker symbols, or build settings."
        ]
    }
    output = Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / "ios" / "portable-source-probe.json"
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(f"Project translation units: {len(sources)}")
    print(f"Files visited through local includes: {len(visited_files)}")
    print(f"Potential candidates (heuristic only): {len(candidates)}")
    print(f"Source files with platform include hits: {len(report['platform_include_hits_by_source'])}")
    print(f"Source files with unresolved nonstandard includes: {len(report['unresolved_nonstandard_includes_by_source'])}")
    print(f"Report written to: {output}")
    return 1 if errors else 0


if __name__ == "__main__":
    raise SystemExit(main())
