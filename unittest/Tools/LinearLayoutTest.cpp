#include "triton/Tools/LinearLayout.h"
#include "triton/Tools/LayoutUtils.h"

#include "mlir/Support/LLVM.h"
#include "llvm/Support/Signals.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace mlir {
std::ostream &operator<<(std::ostream &os, StringAttr str) {
  os << str.str();
  return os;
}
} // namespace mlir

namespace mlir::triton {
namespace {

using ::llvm::to_vector;
using ::testing::ElementsAre;
using ::testing::IsEmpty;
using ::testing::Pair;

using BasesT = LinearLayout::BasesT;

class LinearLayoutTest : public ::testing::Test {
public:
  StringAttr S(StringRef str) { return StringAttr::get(&ctx, str); }

protected:
  MLIRContext ctx;
};

TEST_F(LinearLayoutTest, Empty) {
  LinearLayout layout = LinearLayout::empty();
  EXPECT_THAT(layout.getBases(), IsEmpty());
  EXPECT_THAT(to_vector(layout.getInDimNames()), IsEmpty());
  EXPECT_THAT(to_vector(layout.getOutDimNames()), IsEmpty());
}

TEST_F(LinearLayoutTest, Identity1D) {
  LinearLayout layout =
      LinearLayout::identity1D(32, S("testIns"), S("testOuts"));
  EXPECT_THAT(layout, LinearLayout({{S("testIns"), {{1}, {2}, {4}, {8}, {16}}}},
                                   {S("testOuts")}));
  EXPECT_THAT(to_vector(layout.getInDimNames()), ElementsAre(S("testIns")));
  EXPECT_THAT(to_vector(layout.getOutDimNames()), ElementsAre(S("testOuts")));
  EXPECT_THAT(layout.getInDimSizeLog2(S("testIns")), 5);
  EXPECT_THAT(layout.getOutDimSizeLog2(S("testOuts")), 5);
}

TEST_F(LinearLayoutTest, Identity1DSize1) {
  LinearLayout layout =
      LinearLayout::identity1D(1, S("testIns"), S("testOuts"));
  EXPECT_EQ(layout, LinearLayout({{S("testIns"), {}}}, {S("testOuts")}));
  EXPECT_THAT(to_vector(layout.getInDimNames()), ElementsAre(S("testIns")));
  EXPECT_THAT(to_vector(layout.getOutDimNames()), ElementsAre(S("testOuts")));
  EXPECT_THAT(layout.getInDimSizeLog2(S("testIns")), 0);
  EXPECT_THAT(layout.getOutDimSizeLog2(S("testOuts")), 0);
}

TEST_F(LinearLayoutTest, Zeros1D) {
  LinearLayout layout = LinearLayout::zeros1D(32, S("ins"), S("outs"));
  EXPECT_EQ(layout,
            LinearLayout({{S("ins"), {{0}, {0}, {0}, {0}, {0}}}}, {S("outs")}));
}

TEST_F(LinearLayoutTest, MultiplyIdentity) {
  LinearLayout prod = LinearLayout::identity1D(16, S("in"), S("out")) *
                      LinearLayout::identity1D(32, S("in"), S("out"));
  EXPECT_EQ(prod, LinearLayout(
                      {{S("in"),
                        {{1}, {2}, {4}, {8}, {16}, {32}, {64}, {128}, {256}}}},
                      {S("out")}));
  EXPECT_THAT(to_vector(prod.getInDimNames()), ElementsAre(S("in")));
  EXPECT_THAT(to_vector(prod.getOutDimNames()), ElementsAre(S("out")));
}

TEST_F(LinearLayoutTest, MultiplyDisjoint) {
  LinearLayout prod = LinearLayout::identity1D(32, S("in1"), S("out1")) *
                      LinearLayout::identity1D(16, S("in2"), S("out2"));
  EXPECT_EQ(prod, LinearLayout(
                      {
                          {S("in1"), {{1, 0}, {2, 0}, {4, 0}, {8, 0}, {16, 0}}},
                          {S("in2"), {{0, 1}, {0, 2}, {0, 4}, {0, 8}}},
                      },
                      {S("out1"), S("out2")}));
  EXPECT_THAT(to_vector(prod.getInDimNames()), ElementsAre(S("in1"), S("in2")));
  EXPECT_THAT(to_vector(prod.getOutDimNames()),
              ElementsAre(S("out1"), S("out2")));
}

TEST_F(LinearLayoutTest, MultiplyByEmpty) {
  LinearLayout prod =
      LinearLayout::empty() * LinearLayout::identity1D(32, S("in"), S("out"));
  EXPECT_EQ(prod, LinearLayout::identity1D(32, S("in"), S("out")));
}

TEST_F(LinearLayoutTest, MultiplyByZeros) {
  LinearLayout prod = LinearLayout::identity1D(8, S("in"), S("out")) *
                      LinearLayout::zeros1D(16, S("in"), S("out"));
  EXPECT_EQ(prod, LinearLayout({{S("in"), {{1}, {2}, {4}, {0}, {0}, {0}, {0}}}},
                               {S("out")}));
}

TEST_F(LinearLayoutTest, MultiplyZerosByDegenerate) {
  LinearLayout prod = LinearLayout::zeros1D(16, S("in"), S("out1")) *
                      LinearLayout({{S("in"), {}}}, {S("out2")});
  EXPECT_EQ(prod, LinearLayout({{S("in"), {{0, 0}, {0, 0}, {0, 0}, {0, 0}}}},
                               {S("out1"), S("out2")}));
}

TEST_F(LinearLayoutTest, MultiplyEmptyIdentityAndZeros) {
  LinearLayout prod = LinearLayout::identity1D(0, S("in"), S("out")) *
                      LinearLayout::zeros1D(4, S("in"), S("out"));
  EXPECT_EQ(prod, LinearLayout({{S("in"), {{0}, {0}}}}, {S("out")}));
}

TEST_F(LinearLayoutTest, MultiplyOverlapping) {
  LinearLayout prod = LinearLayout::identity1D(4, S("in"), S("out1")) *
                      LinearLayout::identity1D(8, S("in"), S("out2"));
  EXPECT_EQ(prod,
            LinearLayout({{S("in"), {{1, 0}, {2, 0}, {0, 1}, {0, 2}, {0, 4}}}},
                         {S("out1"), S("out2")}));
}

TEST_F(LinearLayoutTest, TimesEquals) {
  LinearLayout prod = LinearLayout::empty();
  prod *= LinearLayout::identity1D(32, S("in"), S("out"));
  EXPECT_EQ(prod, LinearLayout::identity1D(32, S("in"), S("out")));
}

TEST_F(LinearLayoutTest, GetOutDimSizeLog2) {
  LinearLayout layout(
      {
          {S("in0"), {{0}, {0}, {0}}},
          {S("in1"), {{1}, {2}}},
      },
      {S("dim0")});
  EXPECT_EQ(layout.getOutDimSizeLog2(S("dim0")), 2);
}

TEST_F(LinearLayoutTest, TransposeOuts) {
  LinearLayout layout = (LinearLayout::identity1D(32, S("in1"), S("out1")) *
                         LinearLayout::identity1D(16, S("in2"), S("out2")))
                            .transposeOuts({S("out2"), S("out1")});
  EXPECT_THAT(to_vector(layout.getOutDimNames()),
              ElementsAre(S("out2"), S("out1")));
  EXPECT_EQ(layout,
            LinearLayout(
                {
                    {S("in1"), {{0, 1}, {0, 2}, {0, 4}, {0, 8}, {0, 16}}},
                    {S("in2"), {{1, 0}, {2, 0}, {4, 0}, {8, 0}}},
                },
                {S("out2"), S("out1")}));
}

TEST_F(LinearLayoutTest, TransposeOutsDegenerate) {
  LinearLayout layout = (LinearLayout::identity1D(32, S("in1"), S("out1")) *
                         LinearLayout::identity1D(1, S("in2"), S("out2")))
                            .transposeOuts({S("out2"), S("out1")});
  EXPECT_THAT(to_vector(layout.getOutDimNames()),
              ElementsAre(S("out2"), S("out1")));
  EXPECT_EQ(layout,
            LinearLayout(
                {
                    {S("in1"), {{0, 1}, {0, 2}, {0, 4}, {0, 8}, {0, 16}}},
                    {S("in2"), {}},
                },
                {S("out2"), S("out1")}));
}

TEST_F(LinearLayoutTest, TransposeIns) {
  LinearLayout layout = (LinearLayout::identity1D(32, S("in1"), S("out1")) *
                         LinearLayout::identity1D(16, S("in2"), S("out2")))
                            .transposeIns({S("in2"), S("in1")});
  EXPECT_THAT(to_vector(layout.getInDimNames()),
              ElementsAre(S("in2"), S("in1")));
  EXPECT_EQ(layout,
            LinearLayout(
                {
                    {S("in2"), {{0, 1}, {0, 2}, {0, 4}, {0, 8}}},
                    {S("in1"), {{1, 0}, {2, 0}, {4, 0}, {8, 0}, {16, 0}}},
                },
                {S("out1"), S("out2")}));
}

TEST_F(LinearLayoutTest, EmptyToString) {
  // Mostly I just want to make sure it doesn't crash.
  EXPECT_EQ(LinearLayout::empty().toString(), "\n(empty layout)");
}

TEST_F(LinearLayoutTest, Apply) {
  LinearLayout layout(
      {
          {S("in1"), {{4, 2}, {2, 1}, {1, 0}}},
          {S("in2"), {{1, 2}, {2, 1}}},
      },
      {{S("out1"), 8}, {S("out2"), 4}}, /*requireSurjective=*/false);
  EXPECT_THAT(layout.apply({{S("in1"), 0}, {S("in2"), 0}}),
              ElementsAre(Pair(S("out1"), 0), Pair(S("out2"), 0)));
  EXPECT_THAT(layout.apply({{S("in2"), 0}, {S("in1"), 1}}),
              ElementsAre(Pair(S("out1"), 4), Pair(S("out2"), 2)));
  EXPECT_THAT(layout.apply({{S("in2"), 1}, {S("in1"), 0}}),
              ElementsAre(Pair(S("out1"), 1), Pair(S("out2"), 2)));
}

// This is really more of a benchmark than a test.  We're checking that it
// doesn't take so long to run that a human notices and says "hmm".  :)
TEST_F(LinearLayoutTest, ConstructLargeLayout) {
  std::vector<std::vector<int32_t>> pows2;
  for (int i = 0; i < 25; i++) {
    pows2.emplace_back().push_back(1 << i);
  }
  LinearLayout layout({{S("in"), pows2}}, {S("out")});
  (void)layout;
}

TEST_F(LinearLayoutTest, Compose) {
  LinearLayout l1(
      {
          {S("in1"), {{1, 1}, {0, 1}}},
          {S("in2"), {{1, 0}, {1, 2}}},
      },
      {S("out1"), S("out2")});
  LinearLayout l2(
      {
          {S("out1"), {{2, 2}, {1, 0}}},
          {S("out2"), {{1, 1}, {2, 1}}},
      },
      {S("out3"), S("out4")});
  LinearLayout composition = l1.compose(l2);
  EXPECT_EQ(composition,
            LinearLayout(
                {
                    {S("in1"), {{3, 3}, {1, 1}}},
                    {S("in2"), {{2, 2}, {0, 3}}},
                },
                {{S("out3"), 4}, {S("out4"), 4}}, /*requireSurjective=*/false));
  EXPECT_FALSE(composition.isSurjective());
}

TEST_F(LinearLayoutTest, Compose4D) {
  LinearLayout l1(
      {{S("in0"), {{1, 0, 0, 0}, {2, 0, 0, 0}}},
       {S("in1"), {{4, 0, 0, 0}, {8, 0, 0, 0}, {16, 0, 0, 0}, {32, 0, 0, 0}}},
       {S("in2"), {{0, 0, 1, 0}, {0, 0, 0, 1}, {0, 0, 0, 2}}},
       {S("in3"), {}}},
      {S("out3"), S("out0"), S("out1"), S("out2")});
  LinearLayout l2(
      {
          {S("out3"),
           {{1, 0, 0, 0},
            {2, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}}},
          {S("out0"), {{0, 1, 0, 0}}},
          {S("out1"), {{0, 0, 1, 0}}},
          {S("out2"), {{0, 0, 0, 1}, {0, 0, 0, 2}}},
      },
      {S("out3"), S("out2"), S("out1"), S("out0")});
  EXPECT_EQ(
      l1.compose(l2),
      LinearLayout(
          {
              {S("in0"), {{1, 0, 0, 0}, {2, 0, 0, 0}}},
              {S("in1"),
               {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}},
              {S("in2"), {{0, 0, 1, 0}, {0, 0, 0, 1}, {0, 0, 0, 2}}},
              {S("in3"), {}},
          },
          {{S("out3"), 4}, {S("out2"), 2}, {S("out1"), 2}, {S("out0"), 4}},
          /*requireSurjective=*/false));
}

TEST_F(LinearLayoutTest, ReshapeIns) {
  LinearLayout ll({{S("in1"), {{1}, {4}, {8}}}, {S("in2"), {{2}}}}, {S("out")});
  EXPECT_EQ(ll.reshapeIns({{S("in3"), {2}}, {S("in4"), {8}}}),
            LinearLayout({{S("in3"), {{1}}}, {S("in4"), {{4}, {8}, {2}}}},
                         {S("out")}));
}

TEST_F(LinearLayoutTest, ReshapeInsDegenerateIn) {
  LinearLayout ll({{S("in1"), {{1}, {4}, {2}}}, {S("in2"), {}}}, {S("out")});
  EXPECT_EQ(
      ll.reshapeIns({{S("in3"), {4}}, {S("in4"), {2}}}),
      LinearLayout({{S("in3"), {{1}, {4}}}, {S("in4"), {{2}}}}, {S("out")}));
}

TEST_F(LinearLayoutTest, ReshapeInsDegenerateOut) {
  LinearLayout ll({{S("in1"), {{1}, {4}}}, {S("in2"), {{2}}}}, {S("out")});
  EXPECT_EQ(
      ll.reshapeIns({{S("in3"), {8}}, {S("in4"), {1}}}),
      LinearLayout({{S("in3"), {{1}, {4}, {2}}}, {S("in4"), {}}}, {S("out")}));
}

TEST_F(LinearLayoutTest, ReshapeInsDegenerateFirstOut) {
  LinearLayout ll({{S("in1"), {{1}, {4}}}, {S("in2"), {{2}}}}, {S("out")});
  EXPECT_EQ(
      ll.reshapeIns({{S("in3"), {1}}, {S("in4"), {8}}}),
      LinearLayout({{S("in3"), {}}, {S("in4"), {{1}, {4}, {2}}}}, {S("out")}));
}

TEST_F(LinearLayoutTest, FlattenIns) {
  LinearLayout ll({{S("in1"), {{1}, {4}, {8}}}, {S("in2"), {{2}}}}, {S("out")});
  EXPECT_EQ(ll.flattenIns(),
            LinearLayout({{S("in1"), {{1}, {4}, {8}, {2}}}}, {S("out")}));
}

TEST_F(LinearLayoutTest, FlattenInsEdgeCases) {
  EXPECT_EQ(LinearLayout({{S("in1"), {}}}, {S("out")}).flattenIns(),
            LinearLayout({{S("in1"), {}}}, {S("out")}));
  EXPECT_EQ(LinearLayout({{S("in1"), {}}}, {}).flattenIns(),
            LinearLayout({{S("in1"), {}}}, {}));
  using BasesArray =
      ArrayRef<std::pair<StringAttr, std::vector<std::vector<int32_t>>>>;
  EXPECT_EQ(LinearLayout(BasesArray{}, {S("out")}).flattenIns(),
            LinearLayout(BasesArray{}, {S("out")}));
  EXPECT_EQ(LinearLayout(BasesArray{}, {}).flattenIns(),
            LinearLayout(BasesArray{}, {}));
}

TEST_F(LinearLayoutTest, ReshapeOuts) {
  LinearLayout ll({{S("in1"), {{1}, {4}, {8}}}, {S("in2"), {{3}}}}, {S("out")});
  EXPECT_EQ(ll.getTotalOutDimSize(), 16);
  EXPECT_EQ(
      ll.reshapeOuts({{S("out2"), {2}}, {S("out3"), {8}}}),
      LinearLayout({{S("in1"), {{1, 0}, {0, 2}, {0, 4}}}, {S("in2"), {{1, 1}}}},
                   {S("out2"), S("out3")}));
}

TEST_F(LinearLayoutTest, ReshapeOutsDegenerateIn) {
  LinearLayout ll({{S("in1"), {{1}, {4}, {2}}}, {S("in2"), {}}}, {S("out")});
  EXPECT_EQ(ll.reshapeOuts({{S("out1"), {4}}, {S("out2"), {2}}}),
            LinearLayout({{S("in1"), {{1, 0}, {0, 1}, {2, 0}}}, {S("in2"), {}}},
                         {S("out1"), S("out2")}));
}

TEST_F(LinearLayoutTest, ReshapeOutsDegenerateOut) {
  LinearLayout ll({{S("in1"), {{1}, {4}}}, {S("in2"), {{2}}}}, {S("out")});
  EXPECT_EQ(ll.reshapeOuts({{S("out1"), {8}}, {S("out2"), {1}}}),
            LinearLayout({{S("in1"), {{1, 0}, {4, 0}}}, {S("in2"), {{2, 0}}}},
                         {S("out1"), S("out2")}));
}

TEST_F(LinearLayoutTest, FlattenOuts) {
  LinearLayout ll({{S("in1"), {{1, 0}, {4, 1}, {8, 4}}}, {S("in2"), {{3, 2}}}},
                  {{S("out1"), 16}, {S("out2"), 8}},
                  /*requireSurjective=*/false);
  EXPECT_EQ(ll.flattenOuts(),
            LinearLayout({{S("in1"), {{1}, {4 + 16}, {8 + 4 * 16}}},
                          {S("in2"), {{3 + 2 * 16}}}},
                         {{S("out1"), 16 * 8}}, /*requireSurjective=*/false));
}

TEST_F(LinearLayoutTest, FlattenOutsEdgeCases) {
  EXPECT_EQ(LinearLayout({{S("in1"), {}}}, {S("out")}).flattenOuts(),
            LinearLayout({{S("in1"), {}}}, {S("out")}));
  EXPECT_EQ(LinearLayout({{S("in1"), {}}}, {}).flattenOuts(),
            LinearLayout({{S("in1"), {}}}, {}));
  using BasesArray =
      ArrayRef<std::pair<StringAttr, std::vector<std::vector<int32_t>>>>;
  EXPECT_EQ(LinearLayout(BasesArray{}, {S("out")}).flattenOuts(),
            LinearLayout(BasesArray{}, {S("out")}));
  EXPECT_EQ(LinearLayout(BasesArray{}, {}).flattenOuts(),
            LinearLayout(BasesArray{}, {}));
}

TEST_F(LinearLayoutTest, InvertAndCompose_Simple) {
  LinearLayout l1({{S("in1"), {{2}, {1}, {4}}}}, {S("out")});
  LinearLayout l2({{S("in2"), {{4}, {1}, {2}}}}, {S("out")});

  // Inverse of l2 is
  //   out(1) => in2=2
  //   out(2) => in2=4
  //   out(4) => in2=1.
  //
  // Composing with l1 gives
  //   l2^-1(l1(1)) = l2^-1(2) = 4
  //   l2^-1(l1(2)) = l2^-1(1) = 2
  //   l2^-1(l1(4)) = l2^-1(4) = 1
  LinearLayout composition = l1.invertAndCompose(l2);
  EXPECT_EQ(composition,
            LinearLayout({{S("in1"), {{4}, {2}, {1}}}}, {S("in2")}));
  // L2 ∘ L2^-1 ∘ L1 == L1.
  EXPECT_EQ(composition.compose(l2), l1);
}

TEST_F(LinearLayoutTest, InvertAndComposeLargerA) {
  // Note that dim0 and dim1 are larger in sharedLaoyout
  auto regLayout =
      LinearLayout({{S("register"), {{0, 1}, {0, 2}, {0, 4}, {0, 32}, {32, 0}}},
                    {S("lane"), {{0, 8}, {0, 16}, {1, 0}, {2, 0}, {4, 0}}},
                    {S("warp"), {{8, 0}, {16, 0}}},
                    {S("block"), {}}},
                   {S("dim0"), S("dim1")});
  auto sharedLayout = LinearLayout({{S("offset"),
                                     {{0, 1},
                                      {0, 2},
                                      {0, 4},
                                      {0, 8},
                                      {0, 16},
                                      {0, 32},
                                      {0, 64},
                                      {1, 8},
                                      {2, 16},
                                      {4, 32},
                                      {8, 0},
                                      {16, 0},
                                      {32, 0},
                                      {64, 0},
                                      {128, 0}}},
                                    {S("block"), {}}},
                                   {S("dim0"), S("dim1")});
  auto expected = LinearLayout(
      {{S("register"), {{1, 0}, {2, 0}, {4, 0}, {32, 0}, {4096, 0}}},
       {S("lane"), {{8, 0}, {16, 0}, {136, 0}, {272, 0}, {544, 0}}},
       {S("warp"), {{1024, 0}, {2048, 0}}},
       {S("block"), {}}},
      {{S("offset"), 32768}, {S("block"), 1}}, /*requireSurjective=*/false);
  EXPECT_EQ(regLayout.invertAndCompose(sharedLayout), expected);
  EXPECT_EQ(regLayout.compose(sharedLayout.invert()), expected);
}

TEST_F(LinearLayoutTest, InvertAndCompose_NonInjective) {
  LinearLayout l1({{S("in1"), {{2}, {1}, {4}}}}, {S("out")});
  LinearLayout l2({{S("in2"), {{0}, {2}, {1}, {4}}}}, {S("out")});

  // The pseudo-inverse of l2 is
  //   out(1) => in2=4
  //   out(2) => in2=2
  //   out(4) => in2=8.
  //
  // Composing with l1 gives
  //   l2^-1(l1(1)) = l2^-1(2) = 2
  //   l2^-1(l1(2)) = l2^-1(0) = 4
  //   l2^-1(l1(4)) = l2^-1(4) = 8
  LinearLayout composition = l1.invertAndCompose(l2);
  EXPECT_EQ(composition,
            LinearLayout({{S("in1"), {{2}, {4}, {8}}}}, {{S("in2"), 16}},
                         /*requireSurjective=*/false));
  EXPECT_FALSE(composition.isSurjective());

  // L2 ∘ L2^-1 ∘ L1 == L1.
  EXPECT_EQ(composition.compose(l2), l1);
}

TEST_F(LinearLayoutTest, InvertAndCompose_BroadcastedInDim) {
  LinearLayout l1({{S("in1"), {{2}, {1}, {4}}}, {S("in2"), {{0}}}}, {S("out")});
  LinearLayout l2({{S("in"), {{4}, {1}, {2}}}}, {S("out")});
  // Inverse of l2 is
  //   out(1) = 2
  //   out(2) = 4
  //   out(4) = 1
  //
  // Composing with l1 gives
  //
  //   l2^-1(l1(1, 0)) = l2^-1(2) = 4
  //   l2^-1(l1(2, 0)) = l2^-1(1) = 2
  //   l2^-1(l1(4, 0)) = l2^-1(4) = 1
  //   l2^-1(l1(0, 1)) = l2^-1(0) = 0
  LinearLayout composition = l1.invertAndCompose(l2);
  EXPECT_EQ(composition,
            LinearLayout({{S("in1"), {{4}, {2}, {1}}}, {S("in2"), {{0}}}},
                         {S("in")}));
  EXPECT_EQ(composition.compose(l2), l1);
}

TEST_F(LinearLayoutTest, InvertAndCompose_BroadcastAtBeginningOfSecond) {
  LinearLayout l1({{S("in"), {{1}, {2}, {4}}}}, {S("out")});
  LinearLayout l2({{S("in"), {{0}, {4}, {1}, {2}}}}, {S("out")});
  // Pseudo-inverse of l2 is
  //  out(1) = 4
  //  out(2) = 8
  //  out(4) = 2
  //
  // l1 is the identity, so composing with l1 gives back l2^-1.
  LinearLayout composition = l1.invertAndCompose(l2);
  EXPECT_EQ(composition,
            LinearLayout({{S("in"), {{4}, {8}, {2}}}}, {{S("in"), 16}},
                         /*requireSurjective=*/false));
  EXPECT_EQ(composition.compose(l2), l1);
}

TEST_F(LinearLayoutTest, InvertAndCompose_BroadcastAtEndOfSecond) {
  LinearLayout l1({{S("in1"), {{1}, {2}, {4}}}}, {S("out")});
  LinearLayout l2({{S("in2"), {{4}, {1}, {2}, {0}}}}, {S("out")});
  // Pseudo-inverse of l2 is
  //
  //  out(1) = 2
  //  out(2) = 4
  //  out(4) = 1
  //
  // l1 is the identity, so composing with l1 gives back l2^-1.
  LinearLayout composition = l1.invertAndCompose(l2);
  EXPECT_EQ(composition,
            LinearLayout({{S("in1"), {{2}, {4}, {1}}}}, {{S("in2"), 16}},
                         /*requireSurjective=*/false));
  EXPECT_TRUE(composition.compose(l2).equalIgnoringOutDimSizes(l1));
}

TEST_F(LinearLayoutTest, InvertAndCompose_BroadcastBeginningAndEndOfSecond) {
  LinearLayout l1({{S("in"), {{1}, {2}, {4}}}}, {S("out")});
  LinearLayout l2({{S("in"), {{0}, {4}, {1}, {2}, {0}}}}, {S("out")});
  LinearLayout composition = l1.invertAndCompose(l2);
  EXPECT_EQ(composition,
            LinearLayout({{S("in"), {{4}, {8}, {2}}}}, {{S("in"), 32}},
                         /*requireSurjective=*/false));
  EXPECT_EQ(composition.compose(l2), l1);
}

TEST_F(LinearLayoutTest, InvertAndCompose_Multidim) {
  LinearLayout l1(
      {{S("in1"), {{1, 0}, {0, 1}, {2, 0}, {3, 2}}}, {S("in2"), {{2, 2}}}},
      {S("out1"), S("out2")});
  LinearLayout l2({{S("in3"), {{0, 1}, {1, 0}, {0, 0}, {0, 2}, {2, 1}}}},
                  {S("out2"), S("out1")});

  LinearLayout c1 = l1.invertAndCompose(l2);
  EXPECT_EQ(c1.compose(l2),
            l1.transposeOuts(llvm::to_vector(l2.getOutDimNames())));

  LinearLayout c2 = l2.invertAndCompose(l1);
  EXPECT_EQ(c2.compose(l1),
            l2.transposeOuts(llvm::to_vector(l1.getOutDimNames())));
}

TEST_F(LinearLayoutTest, InvertAndCompose_BroadcastedDims) {
  LinearLayout l1({{S("in1"), {{1}, {2}, {4}}}, {S("in2"), {{0}}}}, {S("out")});
  LinearLayout l2({{S("in3"), {{1}, {2}, {4}}}, {S("in4"), {{0}}}}, {S("out")});
  LinearLayout c = l1.invertAndCompose(l2);
  EXPECT_EQ(c, LinearLayout(
                   {{S("in1"), {{1, 0}, {2, 0}, {4, 0}}}, {S("in2"), {{0, 0}}}},
                   {{S("in3"), 8}, {S("in4"), 2}},
                   /*requireSurjective=*/false));
  EXPECT_EQ(c.compose(l2),
            l1.transposeOuts(llvm::to_vector(l2.getOutDimNames())));
}

TEST_F(LinearLayoutTest, InvertAndCompose_BroadcastedDims2) {
  LinearLayout a({{S("in1"), {{1}, {2}}}, {S("in2"), {{0}}}}, {S("out")});
  LinearLayout b({{S("in3"), {{2}, {1}}}, {S("in4"), {{0}}}}, {S("out")});
  LinearLayout c = a.invertAndCompose(b);
  EXPECT_EQ(c,
            LinearLayout({{S("in1"), {{2, 0}, {1, 0}}}, {S("in2"), {{0, 0}}}},
                         {{S("in3"), 4}, {S("in4"), 2}},
                         /*requireSurjective=*/false));
  EXPECT_EQ(c.compose(b), a.transposeOuts(llvm::to_vector(b.getOutDimNames())));
}

TEST_F(LinearLayoutTest, InvertAndCompose_IdentityInDim) {
  SmallVector<StringAttr> outDims = {S("dim0"), S("dim1"), S("dim2"),
                                     S("dim3"), S("dim4"), S("dim5"),
                                     S("dim6"), S("dim7"), S("dim8")};

  LinearLayout src({{S("register"),
                     {
                         {0, 0, 0, 0, 0, 0, 0, 0, 1},
                         {0, 0, 0, 0, 0, 0, 0, 1, 0},
                     }},
                    {S("lane"),
                     {
                         {0, 0, 0, 0, 0, 0, 1, 0, 0},
                         {0, 0, 0, 0, 0, 1, 0, 0, 0},
                         {0, 0, 0, 0, 1, 0, 0, 0, 0},
                         {0, 0, 0, 1, 0, 0, 0, 0, 0},
                         {0, 0, 1, 0, 0, 0, 0, 0, 0},
                     }},
                    {S("warp"),
                     {
                         {0, 1, 0, 0, 0, 0, 0, 0, 0},
                         {1, 0, 0, 0, 0, 0, 0, 0, 0},
                     }},
                    {S("block"), {}}},
                   outDims);
  LinearLayout dst({{S("register"),
                     {
                         {0, 0, 0, 0, 0, 0, 0, 0, 1},
                         {0, 0, 0, 0, 0, 0, 0, 1, 0},
                     }},
                    {S("lane"),
                     {
                         {1, 0, 0, 0, 0, 0, 0, 0, 0},
                         {0, 1, 0, 0, 0, 0, 0, 0, 0},
                         {0, 0, 1, 0, 0, 0, 0, 0, 0},
                         {0, 0, 0, 1, 0, 0, 0, 0, 0},
                         {0, 0, 0, 0, 1, 0, 0, 0, 0},
                     }},
                    {S("warp"),
                     {
                         {0, 0, 0, 0, 0, 1, 0, 0, 0},
                         {0, 0, 0, 0, 0, 0, 1, 0, 0},
                     }},
                    {S("block"), {}}},
                   outDims);

  LinearLayout cvt = dst.invertAndCompose(src);
  SmallVector<std::pair<StringAttr, int32_t>> k = {
      {S("register"), 3}, {S("lane"), 0}, {S("warp"), 2}, {S("block"), 0}};

  EXPECT_EQ(dst.apply(k), src.apply(cvt.apply(k)));
}

TEST_F(LinearLayoutTest, NumConsecutiveInOut) {
  EXPECT_EQ(
      1,
      LinearLayout::identity1D(1, S("in"), S("out")).getNumConsecutiveInOut());
  EXPECT_EQ(
      4,
      LinearLayout::identity1D(4, S("in"), S("out")).getNumConsecutiveInOut());
  EXPECT_EQ(4, (LinearLayout::identity1D(4, S("in1"), S("out")) *
                LinearLayout::identity1D(8, S("in2"), S("out")))
                   .getNumConsecutiveInOut());
  EXPECT_EQ(4, (LinearLayout::identity1D(4, S("in"), S("out1")) *
                LinearLayout::identity1D(8, S("in"), S("out2")))
                   .getNumConsecutiveInOut());
  EXPECT_EQ(1, (LinearLayout::zeros1D(4, S("in"), S("out1")) *
                LinearLayout::identity1D(4, S("in"), S("out2")))
                   .getNumConsecutiveInOut());
  EXPECT_EQ(1, LinearLayout({{S("in"), {{1}, {2}, {4}, {9}}}}, {S("out")})
                   .getNumConsecutiveInOut());
  EXPECT_EQ(2, LinearLayout({{S("in"), {{1}, {2}, {4}, {10}}}}, {S("out")})
                   .getNumConsecutiveInOut());
  EXPECT_EQ(2, LinearLayout({{S("in"), {{1}, {4}, {2}}}}, {S("out")})
                   .getNumConsecutiveInOut());
  EXPECT_EQ(2, LinearLayout(
                   {
                       {S("in"), {{1}, {2}, {4}}},
                       {S("in2"), {{8}, {18}}},
                   },
                   {S("out")})
                   .getNumConsecutiveInOut());
}

TEST_F(LinearLayoutTest, EqualsChecksOutDimSizes) {
  EXPECT_FALSE(LinearLayout::identity1D(4, S("in"), S("out")) ==
               LinearLayout({{S("in"), {{1}, {2}}}}, {{S("out"), 8}},
                            /*requireSurjective=*/false));
  EXPECT_TRUE(LinearLayout::identity1D(4, S("in"), S("out")) !=
              LinearLayout({{S("in"), {{1}, {2}}}}, {{S("out"), 8}},
                           /*requireSurjective=*/false));
  EXPECT_TRUE(LinearLayout::identity1D(4, S("in"), S("out"))
                  .equalIgnoringOutDimSizes(
                      LinearLayout({{S("in"), {{1}, {2}}}}, {{S("out"), 8}},
                                   /*requireSurjective=*/false)));
}

TEST_F(LinearLayoutTest, Sublayout) {
  LinearLayout l1({{S("in1"), {{1, 0}, {0, 1}, {2, 0}}}, {S("in2"), {{0, 1}}}},
                  {S("out1"), S("out2")});
  EXPECT_EQ(l1.sublayout({S("in1"), S("in2")}, {S("out1")}),
            LinearLayout({{S("in1"), {{1}, {0}, {2}}}, {S("in2"), {{0}}}},
                         {S("out1")}));
  EXPECT_EQ(l1.sublayout({S("in2"), S("in1")}, {S("out1")}),
            LinearLayout({{S("in1"), {{1}, {0}, {2}}}, {S("in2"), {{0}}}},
                         {S("out1")}));
  EXPECT_EQ(l1.sublayout({S("in2"), S("in1")}, {S("out2"), S("out1")}), l1);
  EXPECT_EQ(l1.sublayout({S("in1")}, {S("out1")}),
            LinearLayout({{S("in1"), {{1}, {0}, {2}}}}, {S("out1")}));
  EXPECT_EQ(l1.sublayout({}, {}), LinearLayout::empty());
  EXPECT_EQ(l1.sublayout({S("in1")}, {}),
            LinearLayout({{S("in1"), {{}, {}, {}}}}, {}));
  EXPECT_EQ(l1.sublayout({}, {S("out1")}),
            LinearLayout(LinearLayout::BasesT{}, {{S("out1"), 4}},
                         /*requireSurjective=*/false));
}

TEST_F(LinearLayoutTest, SublayoutIsZero) {
  EXPECT_FALSE(LinearLayout::identity1D(4, S("in"), S("out"))
                   .sublayoutIsZero({S("in")}, {S("out")}));
  EXPECT_TRUE(LinearLayout::identity1D(4, S("in"), S("out"))
                  .sublayoutIsZero({}, {S("out")}));
  EXPECT_TRUE(LinearLayout::identity1D(4, S("in"), S("out"))
                  .sublayoutIsZero({S("in")}, {}));
  EXPECT_TRUE(
      LinearLayout::identity1D(4, S("in"), S("out")).sublayoutIsZero({}, {}));

  LinearLayout l1({{S("in1"), {{0, 1}, {0, 2}}}, {S("in2"), {{1, 1}}}},
                  {S("out1"), S("out2")});
  EXPECT_TRUE(l1.sublayoutIsZero({S("in1")}, {S("out1")}));
  EXPECT_FALSE(l1.sublayoutIsZero({S("in1")}, {S("out2")}));
  EXPECT_FALSE(l1.sublayoutIsZero({S("in2")}, {S("out1")}));
  EXPECT_FALSE(l1.sublayoutIsZero({S("in2")}, {S("out2")}));
}

TEST_F(LinearLayoutTest, FreeVariableMasks) {
  using llvm::to_vector;
  using AR = llvm::ArrayRef<std::pair<StringAttr, int32_t>>;

  EXPECT_EQ(AR(to_vector(LinearLayout::identity1D(4, S("in"), S("out"))
                             .getFreeVariableMasks())),
            AR({{S("in"), 0}}));
  EXPECT_EQ(
      AR(to_vector(
          LinearLayout::zeros1D(16, S("in"), S("out")).getFreeVariableMasks())),
      AR({{S("in"), 0b1111}}));
  EXPECT_EQ(AR(to_vector((LinearLayout::identity1D(2, S("in"), S("out")) *
                          LinearLayout::zeros1D(4, S("in"), S("out")) *
                          LinearLayout::identity1D(4, S("in"), S("out")) *
                          LinearLayout::zeros1D(2, S("in"), S("out")))
                             .getFreeVariableMasks())),
            AR({{S("in"), 0b100110}}));
  EXPECT_EQ(AR(to_vector((LinearLayout::identity1D(2, S("in"), S("out")) *
                          LinearLayout::zeros1D(4, S("in"), S("out")) *
                          LinearLayout::identity1D(4, S("in"), S("out")) *
                          LinearLayout::zeros1D(2, S("in"), S("out")))
                             .getFreeVariableMasks())),
            AR({{S("in"), 0b100110}}));
  EXPECT_EQ(AR(to_vector(LinearLayout({{S("in1"), {{1, 1}, {2, 2}, {0, 0}}},
                                       {S("in2"), {{1, 0}, {0, 1}, {2, 0}}}},
                                      {S("out1"), S("out2")})
                             .getFreeVariableMasks())),
            AR({{S("in1"), 0b100}, {S("in2"), 0b10}}));
}

TEST_F(LinearLayoutTest, QuotientOneDimension) {
  LinearLayout layout(
      {
          {S("dim1"), {{1, 0}}},
          {S("dim2"), {{0, 0}}},
      },
      {{S("dim1"), 2}, {S("dim2"), 1}}, /*requireSurjective=*/false);

  // Quotient over dim1, which is trivial
  auto quotientLayout = layout.quotient({S("dim1")});
  ASSERT_TRUE(quotientLayout.has_value());
  EXPECT_EQ(*quotientLayout, LinearLayout::zeros1D(2, S("dim2"), S("dim2")));
  // dim2 is zero, not the identity
  ASSERT_FALSE(quotientLayout->quotient({S("dim2")}).has_value());
}

TEST_F(LinearLayoutTest, QuotientSeveralDimensions) {
  LinearLayout layout(
      {
          {S("dim1"), {{1, 0}, {2, 0}, {4, 0}}},
          {S("dim2"), {{0, 1}, {0, 2}}},
      },
      {S("dim1"), S("dim2")});

  auto quotientLayout = layout.quotient({S("dim1"), S("dim2")});
  EXPECT_TRUE(quotientLayout.has_value());
}

TEST_F(LinearLayoutTest, QuotientMultipleTrivialDimensions) {
  LinearLayout layout(
      {
          {S("dim1"), {{1, 0, 2}, {2, 0, 1}}},
          {S("dim2"), {{0, 1, 0}, {0, 2, 0}, {0, 4, 0}}},
          {S("dim3"), {{0, 0, 1}, {0, 0, 2}}},
      },
      {S("dim1"), S("dim2"), S("dim3")});

  // Quotient over dim2 is trivial, even if there's some funny business
  // going on in the other dimensions
  auto quotientLayout = layout.quotient({S("dim2")});
  ASSERT_TRUE(quotientLayout.has_value());

  layout = LinearLayout(
      {
          {S("dim1"), {{1, 0, 2}, {2, 0, 1}}},
          {S("dim2"), {{0, 1, 0}, {0, 2, 0}, {0, 4, 0}}},
          {S("dim3"), {{0, 1, 1}, {0, 0, 2}}},
      },
      {S("dim1"), S("dim2"), S("dim3")});

  // As soon as one maps into the dimension being quotiented or out of it
  // (in this case dim3 depends on dim2), we cannot quotient
  quotientLayout = layout.quotient({S("dim2")});
  ASSERT_FALSE(quotientLayout.has_value());
}

TEST_F(LinearLayoutTest, QuotientEmptyLayout) {
  LinearLayout layout = LinearLayout::empty();

  // Quotienting over a dimension that doesn't exist is invalid
  auto quotientLayout = layout.quotient({S("dim1")});
  ASSERT_FALSE(quotientLayout.has_value());
}

TEST_F(LinearLayoutTest, QuotientIdentityMultipleDimensions) {
  // Test quotient on identity layout with multiple dimensions
  LinearLayout layout = LinearLayout::identity1D(8, S("dim1"), S("dim1")) *
                        LinearLayout::identity1D(2, S("dim2"), S("dim2")) *
                        LinearLayout::identity1D(4, S("dim3"), S("dim3"));

  // We can quotient over all dimensions in any order
  auto quotientLayout = layout.quotient({S("dim1"), S("dim3")});
  ASSERT_TRUE(quotientLayout.has_value());
  ASSERT_TRUE(quotientLayout->quotient({S("dim2")}).has_value());
}

LinearLayout getPackedCoordtoPaddedOffset(int M, int KPacked8b, StringAttr row,
                                          StringAttr col, StringAttr offset) {
  std::vector<std::vector<int>> basesRows, basesCols;
  for (int row = 1; row < M; row *= 2) {
    int col = 0;
    int linearCoord = row * KPacked8b + col;
    int offset = (linearCoord / 8) * 16 + (linearCoord % 8);
    basesRows.push_back({offset});
  }

  for (int col = 1; col < KPacked8b; col *= 2) {
    int row = 0;
    int linearCoord = row * KPacked8b + col;
    int offset = (linearCoord / 8) * 16 + (linearCoord % 8);
    basesCols.push_back({offset});
  }

  return LinearLayout({{row, basesRows}, {col, basesCols}},
                      {{offset, M * KPacked8b * 2}}, /*surjective*/ false);
}

TEST_F(LinearLayoutTest, BlackwellMixedPrecisionDotScaledSMEM) {
  std::vector<std::vector<int>> basesRows, basesCols, basesOffset;
  int numFp4Elems = 128;
  int M = 16;
  int KPacked8b = numFp4Elems / M / 2;
  int KPadded8b = numFp4Elems / M;

  for (int offset = 1; offset < M * KPadded8b; offset *= 2) {
    int linearCoordPacked = offset / 16 * 8 + offset % 8;
    int row = linearCoordPacked / KPacked8b;
    int col = linearCoordPacked % KPacked8b;
    basesOffset.push_back({row, col});
  }

  LinearLayout layout({{S("offset"), basesOffset}}, {S("row"), S("col")});
  LinearLayout layoutInverseComputed = layout.pseudoinvert();
  LinearLayout layoutInverseManual = getPackedCoordtoPaddedOffset(
      M, KPacked8b, S("row"), S("col"), S("offset"));

  for (int i = 0; i < M; ++i) {
    for (int j = 0; j < KPacked8b; ++j) {
      auto off1 = layoutInverseManual.apply({{S("row"), i}, {S("col"), j}});
      auto off2 = layoutInverseComputed.apply({{S("row"), i}, {S("col"), j}});
      EXPECT_EQ(off1[0].second, off2[0].second);
    }
  }
}

TEST_F(LinearLayoutTest, BlackwellMixedPrecisionDotScaledSMEMSwizzled) {
  int M = 16;
  int KPadded8b = 128;
  int numFp4Elems = M * KPadded8b;
  int KPacked8b = KPadded8b / 2;
  int elemBitWidth = 8;
  int tileWidthBytes = 128;
  int tileRows = 8;
  int tileCols = 8 * tileWidthBytes / elemBitWidth;
  int vec = 16;

  std::vector<std::vector<int>> bases2D;
  for (int colPadded = 1; colPadded < tileCols; colPadded *= 2) {
    int colPacked = colPadded / 16 * 8 + colPadded % 8;
    bases2D.push_back({0, colPacked});
  }
  for (int row = 1; row < tileRows; row *= 2) {
    int perPhase = 1;
    int maxPhase = 8;
    int colPadded = vec * ((row / perPhase) % maxPhase);
    int colPacked = colPadded / 16 * 8 + colPadded % 8;
    bases2D.push_back({row, colPacked});
  }

  LinearLayout layoutSwizzled({{S("offset"), bases2D}}, {S("row"), S("col")});
  layoutSwizzled = ensureLayoutNotSmallerThan(
      layoutSwizzled, {{S("row"), M}, {S("col"), KPacked8b}});

  auto layoutInverseSwizzled = layoutSwizzled.pseudoinvert();

  LinearLayout layoutInverseNoSwizzle = getPackedCoordtoPaddedOffset(
      M, KPacked8b, S("row"), S("col"), S("offset"));

  for (int i = 0; i < M; ++i) {
    for (int j = 0; j < KPacked8b; ++j) {
      auto nonSwizzleOffset =
          layoutInverseNoSwizzle.apply({{S("row"), i}, {S("col"), j}})[0]
              .second;
      auto swizzledOffset =
          layoutInverseSwizzled.apply({{S("row"), i}, {S("col"), j}})[0].second;
      int row = nonSwizzleOffset / KPadded8b;
      int col = nonSwizzleOffset % KPadded8b;
      int colSwizzled = ((col / 16) ^ (row % 8)) * 16 + col % 16;
      EXPECT_EQ(row * KPadded8b + colSwizzled, swizzledOffset);
    }
  }
}

static SmallVector<StringAttr> makeList(MLIRContext *ctx,
                                        llvm::ArrayRef<llvm::StringRef> list) {
  SmallVector<StringAttr> ret;
  for (auto s : list)
    ret.push_back(StringAttr::get(ctx, s));
  return ret;
}

TEST(SupremumTest, IdenticalLists) {
  MLIRContext ctx;
  SmallVector<StringAttr> x = makeList(&ctx, {"a", "b", "c"});
  SmallVector<StringAttr> y = makeList(&ctx, {"a", "b", "c"});
  EXPECT_EQ(supremum(x, y), x);
}

TEST(SupremumTest, NonUniqueSupremumFirstListPriority) {
  MLIRContext ctx;
  // sup([a, b], [a, c]) should yield [a, b, c]
  SmallVector<StringAttr> x = makeList(&ctx, {"a", "b"});
  SmallVector<StringAttr> y = makeList(&ctx, {"a", "c"});
  EXPECT_EQ(supremum(x, y), makeList(&ctx, {"a", "b", "c"}));
}

TEST(SupremumTest, NonUniqueSupremumAlternate) {
  MLIRContext ctx;
  // sup([a, b], [b, c]) should yield [a, b, c]
  SmallVector<StringAttr> x = makeList(&ctx, {"a", "b"});
  SmallVector<StringAttr> y = makeList(&ctx, {"b", "c"});
  EXPECT_EQ(supremum(x, y), makeList(&ctx, {"a", "b", "c"}));
}

TEST(SupremumTest, DifferentLengths) {
  MLIRContext ctx;
  // sup([a, b, c], [a, d]) should yield [a, b, c, d]
  SmallVector<StringAttr> x = makeList(&ctx, {"a", "b", "c"});
  SmallVector<StringAttr> y = makeList(&ctx, {"a", "d"});
  EXPECT_EQ(supremum(x, y), makeList(&ctx, {"a", "b", "c", "d"}));
}

TEST(SupremumTest, SupremumEmptyLists) {
  MLIRContext ctx;
  SmallVector<StringAttr> x;
  SmallVector<StringAttr> y;
  EXPECT_TRUE(supremum(x, y).empty());
}

TEST(SupremumTest, OneEmptyList) {
  MLIRContext ctx;
  // sup([a, b], []) should yield [a, b]
  SmallVector<StringAttr> x = makeList(&ctx, {"a", "b"});
  SmallVector<StringAttr> y;
  EXPECT_EQ(supremum(x, y), makeList(&ctx, {"a", "b"}));
}

#ifdef LLVM_ENABLE_ASSERTIONS
TEST(SupremumTest, ErrorOnInconsistentOrder) {
  MLIRContext ctx;
  // sup([a, b], [b, a]) has no consistent ordering so it should trigger
  // llvm_unreachable.
  SmallVector<StringAttr> x = makeList(&ctx, {"a", "b"});
  SmallVector<StringAttr> y = makeList(&ctx, {"b", "a"});
  ASSERT_DEATH({ supremum(x, y); }, "Supremum does not exist");
}
#endif

TEST_F(LinearLayoutTest, Divide_Basic) {
  // Test division when A = B * C.
  auto B = LinearLayout::identity1D(8, S("in"), S("out"));
  auto C = LinearLayout::zeros1D(16, S("in"), S("out"));
  auto isC = divideLeft(B * C, B);
  EXPECT_TRUE(isC.has_value());
  EXPECT_EQ(isC.value(), C);
  auto isB = divideRight(B * C, C);
  EXPECT_TRUE(isB.has_value());
  EXPECT_EQ(isB.value(), B);

  isB = divideLeft(C * B, C);
  EXPECT_TRUE(isB.has_value());
  EXPECT_EQ(isB.value(), B);
  isC = divideRight(C * B, B);
  EXPECT_TRUE(isC.has_value());
  EXPECT_EQ(isC.value(), C);
}

TEST_F(LinearLayoutTest, Divide_NonMatchingDims) {
  // If B contains an extra input dimension not present in A, division should
  // fail.
  LinearLayout A = LinearLayout::identity1D(32, S("in"), S("out"));
  LinearLayout B({{S("in"), {{1}, {2}, {4}, {8}}}, {S("extra"), {{0}}}},
                 {S("out")});
  auto candidateOpt = divideLeft(A, B);
  EXPECT_FALSE(candidateOpt.has_value());
  candidateOpt = divideRight(A, B);
  EXPECT_FALSE(candidateOpt.has_value());
}

TEST_F(LinearLayoutTest, Divide_Simple) {
  auto A = LinearLayout::identity1D(8, S("in"), S("out"));
  auto B = LinearLayout::identity1D(4, S("in"), S("out"));
  auto C = LinearLayout::identity1D(2, S("in"), S("out"));
  EXPECT_EQ(divideLeft(A, B), C);
  EXPECT_EQ(divideRight(A, B), C);

  A = LinearLayout::identity1D(8, S("in"), S("out"));
  C = LinearLayout::identity1D(1, S("in"), S("out"));
  EXPECT_EQ(divideLeft(A, A), C);
  EXPECT_EQ(divideRight(A, A), C);
}

TEST_F(LinearLayoutTest, Divide_2D) {
  LinearLayout l1(
      {
          {S("in1"), {{1, 1}, {2, 2}, {0, 8}, {0, 4}}},
          {S("in2"), {{0, 2}, {0, 1}}},
      },
      {S("out1"), S("out2")});
  LinearLayout l2(
      {
          {S("in1"), {{1, 1}, {2, 2}}},
          {S("in2"), {{0, 2}, {0, 1}}},
      },
      {S("out1"), S("out2")});
  LinearLayout l3({{S("in1"), {{0, 2}, {0, 1}}}, {S("in2"), {}}},
                  {S("out1"), S("out2")});
  ASSERT_EQ(l2 * l3, l1);
  ASSERT_EQ(divideLeft(l1, l2).value(), l3);
  ASSERT_EQ(divideRight(l1, l3).value(), l2);
}

TEST_F(LinearLayoutTest, Divide_EliminateInDim) {
  LinearLayout l1(
      {
          {S("in2"), {{0, 1}, {1, 0}}},
          {S("in1"), {{2, 0}, {0, 2}}},
      },
      {S("out1"), S("out2")});
  LinearLayout l2({{S("in2"), {{0, 1}, {1, 0}}}}, {S("out1"), S("out2")});
  LinearLayout l3({{S("in2"), {}}, {S("in1"), {{1, 0}, {0, 1}}}},
                  {S("out1"), S("out2")});
  ASSERT_EQ(l2 * l3, l1);
  EXPECT_EQ(divideLeft(l1, l2).value(), l3);

  l2 = LinearLayout({{S("in2"), {{0, 1}, {1, 0}}}, {S("in1"), {}}},
                    {S("out1"), S("out2")});
  l3 = LinearLayout({{S("in1"), {{1, 0}, {0, 1}}}}, {S("out1"), S("out2")});
  ASSERT_EQ(l2 * l3, l1);
  EXPECT_EQ(divideRight(l1, l3).value(), l2);

  LinearLayout l4({{S("in1"), {{0, 1}, {0, 2}}}, {S("in2"), {}}},
                  {S("out1"), S("out2")});
  LinearLayout l5({{S("in1"), {{0, 1}, {0, 2}}}}, {S("out1"), S("out2")});
  LinearLayout l6({{S("in1"), {}}, {S("in2"), {}}}, {S("out1"), S("out2")});
  ASSERT_EQ(l5 * l6, l4);
  EXPECT_EQ(divideLeft(l4, l5).value(), l6);
  EXPECT_EQ(divideRight(l4, l5).value(), l6);

  LinearLayout l7({{S("in1"), {}}, {S("in2"), {{0, 1}}}, {S("in3"), {}}},
                  {S("out1"), S("out2")});
  LinearLayout l8({{S("in2"), {{0, 1}}}}, {S("out1"), S("out2")});
  LinearLayout l9({{S("in1"), {}}, {S("in2"), {}}, {S("in3"), {}}},
                  {S("out1"), S("out2")});
  ASSERT_EQ(l8 * l9, l7);
  EXPECT_EQ(divideLeft(l7, l8).value(), l9);
  EXPECT_EQ(divideRight(l7, l8).value(), l9);
}

TEST_F(LinearLayoutTest, Divide_EliminateOutDim) {
  LinearLayout l1(
      {
          {S("in2"), {{1, 0}, {1, 0}}},
          {S("in1"), {{2, 0}, {0, 1}}},
      },
      {S("out1"), S("out2")});
  LinearLayout l2({{S("in2"), {{1}, {1}}}}, {S("out1")});
  LinearLayout l3({{S("in2"), {}}, {S("in1"), {{1, 0}, {0, 1}}}},
                  {S("out1"), S("out2")});
  ASSERT_EQ(l2 * l3, l1);
  EXPECT_EQ(divideLeft(l1, l2).value(), l3);

  l1 = LinearLayout(
      {
          {S("in2"), {{1, 0}, {0, 2}}},
          {S("in1"), {{0, 1}, {0, 2}}},
      },
      {S("out1"), S("out2")});
  l2 = LinearLayout({{S("in2"), {{1}}}, {S("in1"), {{1}}}}, {S("out2")});
  l3 = LinearLayout({{S("in2"), {{1, 0}}}, {S("in1"), {{0, 1}}}},
                    {S("out1"), S("out2")});
  ASSERT_EQ(l3 * l2, l1);
  EXPECT_EQ(divideRight(l1, l2).value(), l3);

  LinearLayout l4(
      {
          {S("in1"), {{0, 1}, {0, 2}}},
      },
      {S("out1"), S("out2")});
  using BasesArray =
      ArrayRef<std::pair<StringAttr, std::vector<std::vector<int32_t>>>>;
  LinearLayout l5(BasesArray{}, {S("out1")});
  LinearLayout l6({{S("in1"), {{0, 1}, {0, 2}}}}, {S("out1"), S("out2")});
  ASSERT_EQ(l5 * l6, l4);
  EXPECT_EQ(divideLeft(l4, l5).value(), l6);
  EXPECT_EQ(divideRight(l4, l5).value(), l6);
}

TEST_F(LinearLayoutTest, ColumnActionApplyLayout) {
  // Create a simple LinearLayout with one input dimension "in" and one output
  // "out". The original bases for "in" are: [{1}, {2}, {4}]. According to the
  // ColumnAction example, with action = [2, 0, 1], the new order should be:
  // [{4}, {1}, {2}].
  StringAttr inDim = S("in");
  StringAttr outDim = S("out");
  std::vector<std::vector<int32_t>> origBases = {{1}, {2}, {4}};
  LinearLayout layout({{inDim, origBases}}, {outDim});

  // Construct the ColumnAction: use action vector [2, 0, 1] with inSizeLog2
  // = 3.
  ColumnAction colAction({2, 0, 1}, inDim, 3);
  LinearLayout transformed = colAction.apply(layout);

  // Expected layout: the bases for "in" are permuted to [{4}, {1}, {2}].
  std::vector<std::vector<int32_t>> expectedBases = {{4}, {1}, {2}};
  LinearLayout expectedLayout({{inDim, expectedBases}}, {outDim});

  // Test dropping 4th basis and flipping the other two
  colAction = ColumnAction({1, 0}, inDim, 3);
  transformed = colAction.apply(layout);
  expectedLayout = LinearLayout({{inDim, {{2}, {1}}}}, {{outDim, 8}}, false);
  EXPECT_EQ(transformed, expectedLayout);
}

TEST_F(LinearLayoutTest, ColumnActionApplyValues) {
  // Test that ColumnAction correctly permutes a range of values.
  // We simulate mlir::Value objects via the opaque-pointer mechanism.
  // Create 8 dummy values corresponding to the integers 1..8.
  SmallVector<mlir::Value> values;
  for (int i = 1; i <= 8; ++i) {
    // We use getFromOpaquePointer to make a dummy value that 'carries' the
    // integer i.
    Value val = mlir::Value::getFromOpaquePointer(
        reinterpret_cast<void *>(static_cast<intptr_t>(i)));
    values.push_back(val);
  }

  // Create a ColumnAction with action = [2, 0, 1] and inSizeLog2 = 3.
  // According to the specification, this should permute the value range as:
  //   [x[0], x[4], x[1], x[5], x[2], x[6], x[3], x[7]].
  // Given our dummy values (which represent 1..8), the expected sequence is [1,
  // 5, 2, 6, 3, 7, 4, 8].
  ColumnAction colAction({2, 0, 1}, S("register"), 3);
  SmallVector<mlir::Value> permuted = colAction.apply(values);

  // Extract the integer 'identifier' from each dummy value.
  auto getId = [](mlir::Value val) -> intptr_t {
    return reinterpret_cast<intptr_t>(val.getAsOpaquePointer());
  };
  std::vector<intptr_t> result;
  for (mlir::Value v : permuted)
    result.push_back(getId(v));

  std::vector<intptr_t> expected = {1, 5, 2, 6, 3, 7, 4, 8};
  EXPECT_EQ(result, expected);

  // Test dropping the odd indices
  colAction = ColumnAction({2, 1}, S("register"), 3);
  permuted = colAction.apply(values);
  result.clear();
  for (mlir::Value v : permuted)
    result.push_back(getId(v));

  expected = std::vector<intptr_t>{1, 5, 3, 7};
  EXPECT_EQ(result, expected);
}

} // anonymous namespace
} // namespace mlir::triton

int main(int argc, char *argv[]) {
  llvm::sys::PrintStackTraceOnErrorSignal(argv[0]);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
