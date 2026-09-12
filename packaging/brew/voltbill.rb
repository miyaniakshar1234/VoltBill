class Voltbill < Formula
  desc "Console-Based Electricity Billing and Customer Management System Using C"
  homepage "https://github.com/miyaniakshar1234/VoltBill"
  url "https://github.com/miyaniakshar1234/VoltBill/archive/refs/tags/v2.0.0.tar.gz"
  license "MIT"
  head "https://github.com/miyaniakshar1234/VoltBill.git", branch: "main"

  depends_on "cmake" => :build

  def install
    system "cmake", "-S", ".", "-B", "build", *std_cmake_args
    system "cmake", "--build", "build"
    bin.install "build/voltbill"
  end

  test do
    assert_match "VoltBill v2.0.0", shell_output("#{bin}/voltbill --version")
  end
end
