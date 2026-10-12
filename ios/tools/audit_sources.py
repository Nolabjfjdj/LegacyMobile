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
    report = {
        "report_type": "source_inventory_not_build_validation",
        "repository_root": str(ROOT),
        "projects": [read_project(path) for path in PROJECTS],
        "ios_project_files": sorted(
            str(path.relative_to(ROOT))
            for pattern in ("*.xcodeproj/project.pbxproj", "*.xcworkspace/contents.xcworkspacedata")
            for path in ROOT.rglob(pattern)
            if "Pods" not in path.parts and "build" not in path.parts
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
    print(f"Inventory written to: {output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
