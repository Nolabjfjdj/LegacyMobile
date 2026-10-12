import json
import re
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PROJECTS = [
    ROOT / "Minecraft.Client" / "Minecraft.Client.vcxproj",
    ROOT / "Minecraft.World" / "Minecraft.World.vcxproj",
]
PLATFORM_PATTERNS = {
    "windows": re.compile(r"windows\.h|d3d11\.h|DirectX|WinMain|_WINDOWS64|WIN32", re.I),
    "xbox": re.compile(r"_XBOX|xbox|xdk\.h|xuiapp\.h", re.I),
    "playstation": re.compile(r"__PS3__|__PSVITA__|ORBIS|scebase\.h|cell/", re.I),
    "console_save": re.compile(r"ConsoleSaveFile", re.I),
}
NS = {"msbuild": "http://schemas.microsoft.com/developer/msbuild/2003"}


def read_project(path):
    if not path.is_file():
        return {"path": str(path.relative_to(ROOT)), "exists": False, "error": "project file not found"}
    if path.stat().st_size == 0:
        return {"path": str(path.relative_to(ROOT)), "exists": True, "error": "project file is empty"}
    try:
        root = ET.parse(path).getroot()
    except ET.ParseError as error:
        return {"path": str(path.relative_to(ROOT)), "exists": True, "error": f"invalid XML: {error}"}
    configurations = sorted({
        item.attrib.get("Include", "")
        for item in root.findall(".//msbuild:ProjectConfiguration", NS)
    })
    sources = sorted({
        item.attrib.get("Include", "")
        for item in root.findall(".//msbuild:ClCompile", NS)
        if item.attrib.get("Include")
    })
    headers = sorted({
        item.attrib.get("Include", "")
        for item in root.findall(".//msbuild:ClInclude", NS)
        if item.attrib.get("Include")
    })
    joined = "\n".join([str(path), *sources, *headers])
    return {
        "path": str(path.relative_to(ROOT)),
        "exists": True,
        "configurations": configurations,
        "cpp_source_count": len(sources),
        "header_count": len(headers),
        "source_files": sources,
        "platform_matches": {
            name: [line for line in joined.splitlines() if pattern.search(line)]
            for name, pattern in PLATFORM_PATTERNS.items()
        },
    }


def main():
    platform_headers = {}
    for path in (
        ROOT / "Minecraft.Client" / "stdafx.h",
        ROOT / "Minecraft.World" / "stdafx.h",
    ):
        if path.is_file():
            lines = path.read_text(encoding="utf-8-sig", errors="replace").splitlines()
            platform_headers[str(path.relative_to(ROOT))] = [
                {"line": index, "text": line.strip()}
                for index, line in enumerate(lines, start=1)
                if re.search(
                    r"windows\\.h|d3d11\\.h|DirectXMath|xdk\\.h|xuiapp\\.h|"
                    r"scebase\\.h|Ps3Types\\.h|OrbisTypes\\.h|PSVitaTypes\\.h|"
                    r"__PS3__|__PSVITA__|_WINDOWS64|_DURANGO|_XBOX",
                    line,
                    re.I,
                )
            ]
    report = {
        "report_type": "source_inventory_not_build_validation",
        "repository_root": str(ROOT),
        "projects": [read_project(path) for path in PROJECTS],
        "platform_header_matches": platform_headers,
        "ios_project_files": sorted(
            str(path.relative_to(ROOT))
            for path in ROOT.rglob("*")
            if path.is_dir()
            and path.suffix in {".xcodeproj", ".xcworkspace"}
            and "Pods" not in path.parts
            and "build" not in path.parts
        ),
    }
    output = Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / "ios" / "source-inventory.json"
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    for project in report["projects"]:
        if project.get("error"):
            print(f"WARNING: {project['path']}: {project['error']}")
        else:
            print(f"{project['path']}: {project['cpp_source_count']} C++ sources, {project['header_count']} headers")
    print(f"iOS project files found: {len(report['ios_project_files'])}")
    for path, matches in report["platform_header_matches"].items():
        print(f"{path}: {len(matches)} platform-specific lines to review")
    print(f"Inventory written to: {output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
