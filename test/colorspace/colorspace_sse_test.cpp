#ifdef ZIMG_X86

#include <cmath>
#include "common/cpuinfo.h"
#include "common/pixel.h"
#include "graph/image_filter.h"
#include "colorspace/colorspace.h"

#include "gtest/gtest.h"
#include "graph/filter_validator.h"

namespace {

void test_case(const zimg::colorspace::ColorspaceDefinition &csp_in, const zimg::colorspace::ColorspaceDefinition &csp_out,
               const char * const expected_sha1[3], double expected_snr)
{
	const unsigned w = 640;
	const unsigned h = 480;

	if (!zimg::query_x86_capabilities().sse) {
		SUCCEED() << "sse not available, skipping";
		return;
	}

	zimg::PixelFormat format = zimg::PixelType::FLOAT;
	auto builder = zimg::colorspace::ColorspaceConversion{ w, h, }.
		set_csp_in(csp_in).
		set_csp_out(csp_out);

	auto filter_c = builder.set_cpu(zimg::CPUClass::CPU_NONE).create();
	auto filter_sse = builder.set_cpu(zimg::CPUClass::CPU_X86_SSE).create();

	validate_filter(filter_sse.get(), w, h, format, expected_sha1);
	validate_filter_reference(filter_sse.get(), filter_sse.get(), w, h, format, expected_snr);
}

} // namespace


TEST(ColorspaceConversionSSETest, test_matrix)
{
	using namespace zimg::colorspace;

	const char *expected_sha1[3] = {
		"0495adab9c82d98e73841e229a9b2041838fc0f2",
		"ece7edb1118d4b3063ad80f5d8febb6db7e9633a",
		"73a9ee951c7bde9ae0ada9b90afd1f7ce8b604df"
	};
	const double expected_snr = INFINITY;

	test_case({ MatrixCoefficients::MATRIX_RGB, TransferCharacteristics::TRANSFER_UNSPECIFIED, ColorPrimaries::PRIMARIES_UNSPECIFIED },
	          { MatrixCoefficients::MATRIX_709, TransferCharacteristics::TRANSFER_UNSPECIFIED, ColorPrimaries::PRIMARIES_UNSPECIFIED },
	          expected_sha1, expected_snr);
}

#endif // ZIMG_X86
