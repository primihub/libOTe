load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")

http_archive(
    name = "rules_foreign_cc",
    sha256 = "484fc0e14856b9f7434072bc2662488b3fe84d7798a5b7c92a1feb1a0fa8d088",
    strip_prefix = "rules_foreign_cc-0.8.0",
    #url = "https://github.com/bazelbuild/rules_foreign_cc/archive/0.8.0.tar.gz",
    url = "https://primihub.oss-cn-beijing.aliyuncs.com/tools/rules_foreign_cc_cn-0.8.0.tar.gz",
)
load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")
rules_foreign_cc_dependencies()

new_git_repository(
  name = "toolkit_relic",
  build_file = "//bazel:BUILD.relic",
  remote = "https://gitee.com/orzmzp/relic.git",
  commit = "3f616ad64c3e63039277b8c90915607b6a2c504c",
  shallow_since = "1581106153 -0800",
)

git_repository(
  name = "com_github_nelhage_rules_boost",
  commit = "81945736a62fa8490d2ab6bb31705bb04ce4bb6c",
  #branch = "master",
  remote = "https://gitee.com/primihub/rules_boost.git",
  # shallow_since = "1591047380 -0700",
)
load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
boost_deps()

#deps relic
git_repository(
  name = "ladnir_cryptoTools",
  #branch = "bazel_branch",
  commit = "c3fb58a11f6ac1fc2cd4d8c62c081e13987deef8",
  remote = "https://gitee.com/primihub/cryptoTools.git",
)
