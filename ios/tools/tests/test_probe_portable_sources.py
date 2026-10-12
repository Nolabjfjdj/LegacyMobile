import importlib.util
import unittest
from pathlib import Path

SCRIPT = Path(__file__).resolve().parents[1] / "probe_portable_sources.py"
SPEC = importlib.util.spec_from_file_location("probe_portable_sources", SCRIPT)
PROBE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(PROBE)


class ActiveSourceLinesTests(unittest.TestCase):
    def active(self, source):
        return PROBE.active_source_lines(source.splitlines())

    def test_inactive_windows_include_is_removed_for_ios(self):
        lines = self.active(
            "#ifdef _WINDOWS64\n#include <Windows.h>\n#else\n#include \"Portable.h\"\n#endif"
        )
        self.assertEqual(lines, ['#include "Portable.h"'])

    def test_ios_branch_is_kept_and_legacy_branch_removed(self):
        lines = self.active(
            "#if defined(__APPLE__)\n#include <TargetConditionals.h>\n"
            "#elif defined(_DURANGO)\n#include <xdk.h>\n#endif"
        )
        self.assertEqual(lines, ["#include <TargetConditionals.h>"])

    def test_unknown_condition_keeps_both_branches_conservatively(self):
        lines = self.active(
            "#if FEATURE_SWITCH\n#include \"FeatureA.h\"\n"
            "#else\n#include \"FeatureB.h\"\n#endif"
        )
        self.assertEqual(lines, ['#include "FeatureA.h"', '#include "FeatureB.h"'])

    def test_nested_platform_condition(self):
        lines = self.active(
            "#ifdef _WINDOWS64\n#if FEATURE_SWITCH\n#include <Windows.h>\n"
            "#endif\n#else\n#include \"Common.h\"\n#endif"
        )
        self.assertEqual(lines, ['#include "Common.h"'])

    def test_non_ios_macro_negative_condition(self):
        lines = self.active(
            "#if !defined(__PS3__)\n#include \"Common.h\"\n"
            "#else\n#include <Ps3Types.h>\n#endif"
        )
        self.assertEqual(lines, ['#include "Common.h"'])


if __name__ == "__main__":
    unittest.main()
