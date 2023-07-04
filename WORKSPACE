load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")

http_archive(
  name = "ladnir_cryptoTools",
  sha256 = "59db69f3f34332b5db3ee07d4496c4e5df4eab7a6437c5f45098c4438bd8640d",
  strip_prefix = "cryptoTools",
  urls = [
    "file:///home/cuibo/workspace/cryptoTools_aby3_dep.tar.gz",
  ],
)

load("@ladnir_cryptoTools//bazel:preload.bzl", "cryptoTools_preload")
cryptoTools_preload("ladnir_cryptoTools")

load("@ladnir_cryptoTools//bazel:deps.bzl", "cryptoTools_deps")
cryptoTools_deps()
