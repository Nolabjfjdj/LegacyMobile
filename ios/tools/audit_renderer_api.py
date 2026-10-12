import json
import re
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CLIENT = ROOT / "Minecraft.Client"
WRAPPER = CLIENT / "glWrapper.cpp"
RENDER_HEADERS = [
    CLIENT / "Windows64" / "4JLibs" / "inc" / "4J_Render.h",
]
CALL_PATTERN = re.compile(r"\bRenderManager\s*\.\s*([A-Za-z_]\w*)\s*\(")
SOURCE_SUFFIXES = {".cpp", ".c", ".h", ".hpp"}


def read_text(path):
    return path.read_text(encoding="utf-8-sig", errors="replace")


def main():
    if not WRAPPER.is_file():
        raise SystemExit(f"Required renderer wrapper is missing: {WRAPPER}")

    wrapper_text = read_text(WRAPPER)
    wrapper_methods = Counter(CALL_PATTERN.findall(wrapper_text))
    client_calls = Counter()
    call_sites = {}

    for path in sorted(CLIENT.rglob("*")):
        if not path.is_file() or path.suffix.lower() not in SOURCE_SUFFIXES:
            continue
        if "Windows64/4JLibs" in path.as_posix():
            continue
        text = read_text(path)
        matches = list(CALL_PATTERN.finditer(text))
        if not matches:
            continue
        relative = path.relative_to(ROOT).as_posix()
        for match in matches:
            method = match.group(1)
            client_calls[method] += 1
            call_sites.setdefault(method, set()).add(relative)

    headers = []
    for path in RENDER_HEADERS:
        if path.is_file():
            content = read_text(path)
            headers.append({
                "path": path.relative_to(ROOT).as_posix(),
                "exists": True,
                "d3d11_references": len(re.findall(r"\b(?:ID3D11\w+|D3D11_\w+|IDXGI\w+|HRESULT)\b", content)),
                "declaration_lines": [
                    {"line": number, "text": line.strip()}
                    for number, line in enumerate(content.splitlines(), 1)
                    if re.search(r"\bclass\s+C4JRender\b|\b(?:ID3D11\w+|D3D11_\w+|IDXGI\w+|HRESULT)\b", line)
                ],
            })
        else:
            headers.append({
                "path": path.relative_to(ROOT).as_posix(),
                "exists": False,
            })

    methods = []
    for method in sorted(set(wrapper_methods) | set(client_calls)):
        methods.append({
            "name": method,
            "wrapper_call_count": wrapper_methods[method],
            "client_call_count": client_calls[method],
            "referencing_files": sorted(call_sites.get(method, set())),
        })

    report = {
        "report_type": "renderer_api_dependency_inventory_not_build_validation",
        "wrapper": WRAPPER.relative_to(ROOT).as_posix(),
        "wrapper_method_count": len(wrapper_methods),
        "client_method_count": len(client_calls),
        "methods": methods,
        "renderer_headers": headers,
        "notes": [
            "This inventory records call sites; it does not prove a method has a portable implementation.",
            "The existing wrapper forwards to RenderManager and is not itself a renderer backend.",
            "A Metal backend must implement the methods used by the selected engine path, not merely declare them."
        ],
    }
    output = Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / "ios" / "renderer-api-inventory.json"
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(f"Renderer methods referenced by wrapper: {len(wrapper_methods)}")
    print(f"Renderer methods referenced by client sources: {len(client_calls)}")
    print(f"Renderer headers checked: {len(headers)}")
    print(f"Inventory written to: {output}")


if __name__ == "__main__":
    main()
