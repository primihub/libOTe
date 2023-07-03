load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")


def libote_preload(repo_reference = ""):
  reference_index = ""
  if repo_reference:
      reference_index = "@{}".format(repo_reference)

  if "ladnir_cryptoTools" not in native.existing_rules():
    http_archive(
      name = "ladnir_cryptoTools",
      strip_prefix = "cryptoTools",
      urls = [
        "https://primihub.oss-cn-beijing.aliyuncs.com/tools/cryptoTools_aby3_dep.tar.gz",
      ],
    )
