load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")

local_repository(
  name = "ladnir_cryptoTools",
  path = "/home/cuibo/workspace/cryptoTools",
)

load("@ladnir_cryptoTools//bazel:preload.bzl", "cryptoTools_preload")
cryptoTools_preload("ladnir_cryptoTools")

load("@ladnir_cryptoTools//bazel:deps.bzl", "cryptoTools_deps")
cryptoTools_deps()