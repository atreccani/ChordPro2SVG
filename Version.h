// http://semver.org/
// Given a version number MAJOR.MINOR.PATCH, increment the:
// 1.MAJOR version when you make incompatible API changes,
// 2.MINOR version when you add functionality in a backwards-compatible manner, and
// 3.PATCH version when you make backwards-compatible bug fixes.
//
// Additional labels for pre-release and build metadata are available as extensions to the MAJOR.MINOR.PATCH format.

#define STR_HELPER(x)					#x
#define STR(x)							STR_HELPER(x)

#define VERSION_STR_NAME				"ChordPro2SVG"

#define VERSION_NUM_MAJOR				0
#define VERSION_NUM_MINOR				1
#define VERSION_NUM_PATCH				0
#define VERSION_NUM_REVISION			0

#define VERSION_STR_MAJOR				STR(VERSION_NUM_MAJOR)
#define VERSION_STR_MINOR				STR(VERSION_NUM_MINOR)
#define VERSION_STR_PATCH				STR(VERSION_NUM_PATCH)
#define VERSION_STR_REVISION			STR(VERSION_NUM_REVISION)

#define VERSION_STR_N3					VERSION_STR_MAJOR "." VERSION_STR_MINOR "." VERSION_STR_PATCH "\0"
#define VERSION_STR_N4					VERSION_STR_MAJOR "." VERSION_STR_MINOR "." VERSION_STR_PATCH "." VERSION_STR_REVISION "\0"

#define VERSION_STR_DESCRIPTION			"Command line converter from ChordPro (lyrics + chords) to SVG (vectorial image)."

#define VERSION_STR_FILENAME			VERSION_STR_NAME ".exe"
#define VERSION_STR_COMPANYNAME			""
#define VERSION_STR_COPYRIGHT			"Copyright © 2017 Andrea Treccani"
#define VERSION_STR_TRADEMARKS			"GNU General Public License (GPL)"
#define VERSION_STR_COMPANYDOMAIN		"https://github.com/atreccani/ChordPro2SVG"

