/*
Copyright (c) 2015 - present Advanced Micro Devices, Inc. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <sstream>
#include <regex>
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Support/Path.h"
#include "CUDA2HIP.h"
#include "CUDA2HIP_Scripting.h"
#include "ArgParse.h"
#include "StringUtils.h"
#include "LLVMCompat.h"
#include "Statistics.h"

namespace perl {

  using namespace std;
  using namespace llvm;

  const string sCopyright =
    "##\n"
    "# Copyright (c) 2015-present Advanced Micro Devices, Inc. All rights reserved.\n"
    "#\n"
    "# Permission is hereby granted, free of charge, to any person obtaining a copy\n"
    "# of this software and associated documentation files (the \"Software\"), to deal\n"
    "# in the Software without restriction, including without limitation the rights\n"
    "# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n"
    "# copies of the Software, and to permit persons to whom the Software is\n"
    "# furnished to do so, subject to the following conditions:\n"
    "#\n"
    "# The above copyright notice and this permission notice shall be included in\n"
    "# all copies or substantial portions of the Software.\n"
    "#\n"
    "# THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
    "# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
    "# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE\n"
    "# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
    "# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
    "# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN\n"
    "# THE SOFTWARE.\n"
    "##\n";

  const string sImportant = "# IMPORTANT: Do not change this file manually: it is generated by hipify-clang --perl";
  const string tab = "    ";
  const string tab_2 = tab + tab;
  const string tab_3 = tab_2 + tab;
  const string tab_4 = tab_3 + tab;
  const string tab_5 = tab_4 + tab;
  const string tab_6 = tab_5 + tab;
  const string tab_7 = tab_6 + tab;

  const string endl_2 = "\n\n";
  const string endl_tab = "\n" + tab;
  const string endl_tab_2 = "\n" + tab_2;
  const string endl_tab_3 = "\n" + tab_3;
  const string endl_tab_4 = "\n" + tab_4;
  const string endl_tab_5 = "\n" + tab_5;
  const string endl_tab_6 = "\n" + tab_6;

  const string sub = "sub ";
  const string my = "my ";
  const string my_k = my + "$k = 0;";
  const string return_0 = "return 0;\n";
  const string return_k = "return $k;\n";
  const string while_ = "while ";
  const string unless_ = "unless ";
  const string foreach = "foreach ";
  const string foreach_func = foreach + "$func (\n";
  const string print = "print STDERR ";
  const string printf = "printf STDERR ";
  const string no_warns = "no warnings qw/uninitialized/;";
  const string hipify_perl = "hipify-perl";
  const string warning = "warning: $fileName:$line_num: ";

  const string sCudaDevice = "cudaDevice";
  const string sCudaDeviceId = "cudaDeviceId";
  const string sCudaDevices = "cudaDevices";
  const string sCudaDevice_t = "cudaDevice_t";
  const string sCudaIDs = "cudaIDs";
  const string sCudaGridDim = "cudaGridDim";
  const string sCudaDimGrid = "cudaDimGrid";
  const string sCudaDimBlock = "cudaDimBlock";
  const string sCudaGradInput = "cudaGradInput";
  const string sCudaGradOutput = "cudaGradOutput";
  const string sCudaInput = "cudaInput";
  const string sCudaOutput = "cudaOutput";
  const string sCudaOprod = "cudaOprod";
  const string sCudaOprod_ex = "cudaOprod_ex";
  const string sCudaLongLinkOprod = "cudaLongLinkOprod";
  const string sCudaLongLinkOprod_ex = "cudaLongLinkOprod_ex";
  const string sCudaQuark = "cudaQuark";
  const string sCudaIndices = "cudaIndices";
  const string sCudaGaugeField = "cudaGaugeField";
  const string sCudaQdpGauge = "cudaQdpGauge";
  const string sCudaCpsGauge = "cudaCpsGauge";
  const string sCudaMom = "cudaMom";
  const string sCudaGauge = "cudaGauge";
  const string sCudaGauge_ex = "cudaGauge_ex";
  const string sCudaInGauge = "cudaInGauge";
  const string sCudaInGaugeEx = "cudaInGaugeEx";
  const string sCudaOutGauge = "cudaOutGauge";
  const string sCudaGaugeTemp = "cudaGaugeTemp";
  const string sCudaSpinorOut = "cudaSpinorOut";
  const string sCudaSpinor = "cudaSpinor";
  const string sCudaColorSpinorField = "cudaColorSpinorField";
  const string sCudaInLink = "cudaInLink";
  const string sCudaInLinkEx = "cudaInLinkEx";
  const string sCudaSiteLink = "cudaSiteLink";
  const string sCudaFatLink = "cudaFatLink";
  const string sCudaLongLink = "cudaLongLink";
  const string sCudaUnitarizedLink = "cudaUnitarizedLink";
  const string sCudaULink = "cudaULink";
  const string sCudaStaple = "cudaStaple";
  const string sCudaCloverField = "cudaCloverField";
  const string sCudaParam = "cudaParam";
  const string sCudaForce = "cudaForce";
  const string sCudaInForce = "cudaInForce";
  const string sCudaForce_ex = "cudaForce_ex";
  const string sCudaOutForce = "cudaOutForce";
  const string sCudaResult = "cudaResult";
  const string sCudaMemcpys ="cudaMemcpys";
  const string sCudaRitzVectors = "cudaRitzVectors";
  const string sCudaEigValueSet = "cudaEigValueSet";
  const string sCudaEigVecSet = "cudaEigVecSet";
  
  const set<string> Whitelist{
    {sCudaDevice}, {sCudaDevice_t}, {sCudaIDs}, {sCudaGridDim}, {sCudaDimGrid}, {sCudaDimBlock}, {sCudaDeviceId}, {sCudaDevices},
    {sCudaGradInput}, {sCudaGradOutput}, {sCudaInput}, {sCudaOutput}, {sCudaIndices}, {sCudaMom}, {sCudaGauge},
    {sCudaInGauge}, {sCudaSpinorOut}, {sCudaSpinor}, {sCudaColorSpinorField}, {sCudaSiteLink}, {sCudaFatLink}, {sCudaStaple}, {sCudaCloverField}, {sCudaParam}, {sCudaOutForce}, {sCudaGaugeTemp}, {sCudaResult}, {sCudaForce}, {sCudaInForce}, {sCudaForce_ex}, {sCudaOprod_ex}, {sCudaOutGauge}, {sCudaULink}, {sCudaOprod}, {sCudaGauge_ex}, {sCudaQuark}, {sCudaLongLinkOprod_ex}, {sCudaInGaugeEx}, {sCudaLongLink}, {sCudaUnitarizedLink}, {sCudaQdpGauge}, {sCudaCpsGauge}, {sCudaInLink}, {sCudaInLinkEx}, {sCudaGaugeField}, {sCudaMemcpys}, {sCudaRitzVectors}, {sCudaEigValueSet}, {sCudaLongLinkOprod}, {sCudaEigVecSet}
  };

  void generateHeader(unique_ptr<ostream> &streamPtr) {
    *streamPtr.get() << "#!/usr/bin/perl -w" << endl_2;
    *streamPtr.get() << sCopyright << endl;
    *streamPtr.get() << sImportant << endl_2;
    *streamPtr.get() << "#usage " << hipify_perl << " [OPTIONS] INPUT_FILE" << endl_2;
    *streamPtr.get() << "use Getopt::Long;" << endl;
    *streamPtr.get() << "use File::Basename;" << endl;
    *streamPtr.get() << my << "$whitelist = \"\";" << endl;
    *streamPtr.get() << my << "$exclude_dirs =  \"\";" << endl;
    *streamPtr.get() << my << "$exclude_files = \"\";" << endl;
    *streamPtr.get() << my << "$fileName = \"\";" << endl;
    *streamPtr.get() << my << "%ft;" << endl;
    *streamPtr.get() << my << "%Tkernels;" << endl_2;
    *streamPtr.get() << "GetOptions(" << endl;
    *streamPtr.get() << tab << "  \"examine\" => \\$examine                  # Combines -no-output and -print-stats options." << endl;
    *streamPtr.get() << tab << ", \"inplace\" => \\$inplace                  # Modify input file inplace, replacing input with hipified output, save backup in .prehip file." << endl;
    *streamPtr.get() << tab << ", \"no-output\" => \\$no_output              # Don't write any translated output to stdout." << endl;
    *streamPtr.get() << tab << ", \"print-stats\" => \\$print_stats          # Print translation statistics." << endl;
    *streamPtr.get() << tab << ", \"quiet-warnings\" => \\$quiet_warnings    # Don't print warnings on unknown CUDA functions." << endl;
    *streamPtr.get() << tab << ", \"whitelist=s\" => \\$whitelist            # Whitelist of identifiers." << endl;
    *streamPtr.get() << tab << ", \"exclude-dirs=s\" => \\$exclude_dirs      # Exclude directories." << endl;
    *streamPtr.get() << tab << ", \"exclude-files=s\" => \\$exclude_files    # Exclude files." << endl;
    *streamPtr.get() << ");" << endl_2;
    *streamPtr.get() << "$print_stats = 1 if $examine;" << endl;
    *streamPtr.get() << "$no_output = 1 if $examine;" << endl_2;
    *streamPtr.get() << "# Whitelist of cuda[A-Z] identifiers, which are commonly used in CUDA sources but don't map to any CUDA API:" << endl;
    *streamPtr.get() << "@whitelist = (";
    unsigned int num = 0;
    for (const string &m : Whitelist) {
      *streamPtr.get() << endl_tab << (num ? ", " : "  ") << "\"" << m << "\"";
      ++num;
    }
    *streamPtr.get() << endl << ");" << endl_2;
    *streamPtr.get() << "push(@whitelist, split(',', $whitelist));" << endl;
    *streamPtr.get() << "push(@exclude_dirlist, split(',', $exclude_dirs));" << endl;
    *streamPtr.get() << "push(@exclude_filelist, split(',', $exclude_files));" << endl_2;
    *streamPtr.get() << "# Turn exclude dirlist and exclude_filelist into hash maps" << endl;
    *streamPtr.get() << "\%exclude_dirhash = map { $_ => 1 } @exclude_dirlist;" << endl;
    *streamPtr.get() << "\%exclude_filehash = map { $_ => 1 } @exclude_filelist;" << endl_2;

  }

  void generateStatFunctions(unique_ptr<ostream> &streamPtr) {
    *streamPtr.get() << endl << sub << "totalStats" << " {" << endl;
    *streamPtr.get() << tab << my << "%count = %{shift()};" << endl;
    *streamPtr.get() << tab << my << "$total = 0;" << endl;
    *streamPtr.get() << tab << foreach << "$key (keys %count) {" << endl;
    *streamPtr.get() << tab_2 << "$total += $count{$key};" << endl_tab << "}" << endl;
    *streamPtr.get() << tab << "return $total;" << endl << "};" << endl;
    *streamPtr.get() << endl << sub << "printStats" << " {" << endl;
    *streamPtr.get() << tab << my << "$label     = shift();" << endl;
    *streamPtr.get() << tab << my << "@statNames = @{shift()};" << endl;
    *streamPtr.get() << tab << my << "%counts    = %{shift()};" << endl;
    *streamPtr.get() << tab << my << "$warnings  = shift();" << endl;
    *streamPtr.get() << tab << my << "$loc       = shift();" << endl;
    *streamPtr.get() << tab << my << "$total     = totalStats(\\%counts);" << endl;
    *streamPtr.get() << tab << printf << "\"%s %d CUDA->HIP refs ( \", $label, $total;" << endl;
    *streamPtr.get() << tab << foreach << "$stat (@statNames) {" << endl;
    *streamPtr.get() << tab_2 << printf << "\"%s:%d \", $stat, $counts{$stat};" << endl_tab << "}" << endl;
    *streamPtr.get() << tab << printf << "\")\\n  warning:%d LOC:%d\", $warnings, $loc;" << endl << "}" << endl;
    for (int i = 0; i < 2; ++i) {
      *streamPtr.get() << endl << sub << (i ? "clearStats" : "addStats") << " {" << endl;
      *streamPtr.get() << tab << my << "$dest_ref  = shift();" << endl;
      *streamPtr.get() << tab << my << (i ? "@statNames = @{shift()};" : "%adder     = %{shift()};") << endl;
      *streamPtr.get() << tab << foreach << (i ? "$stat(@statNames)" : "$key (keys %adder)") << " {" << endl;
      *streamPtr.get() << tab_2 << "$dest_ref->" << (i ? "{$stat} = 0;" : "{$key} += $adder{$key};") << endl_tab << "}" << endl << "}" << endl;
    }
  }

  void generateSimpleSubstitutions(unique_ptr<ostream> &streamPtr) {
    *streamPtr.get() << endl << sub << "simpleSubstitutions" << " {" << endl;
    for (int i = 0; i < NUM_CONV_TYPES; ++i) {
      if (i == CONV_INCLUDE_CUDA_MAIN_H || i == CONV_INCLUDE) {
        for (auto &ma : CUDA_INCLUDE_MAP) {
          if (Statistics::isUnsupported(ma.second)) continue;
          if (i == ma.second.type) {
            string sCUDA = ma.first.str();
            string sHIP = ma.second.hipName.str();
            sCUDA = regex_replace(sCUDA, regex("/"), "\\/");
            sHIP = regex_replace(sHIP, regex("/"), "\\/");
            *streamPtr.get() << tab << "$ft{'" << counterNames[ma.second.type] << "'} += s/\\b" << sCUDA << "\\b/" << sHIP << "/g;" << endl;
          }
        }
      } else {
        for (auto &ma : CUDA_RENAMES_MAP()) {
          if (Statistics::isUnsupported(ma.second)) continue;
          if (i == ma.second.type) {
            *streamPtr.get() << tab << "$ft{'" << counterNames[ma.second.type] << "'} += s/\\b" << ma.first.str() << "\\b/" << ma.second.hipName.str() << "/g;" << endl;
          }
        }
      }
    }
    *streamPtr.get() << "}" << endl;
  }

  void generateExternShared(unique_ptr<ostream> &streamPtr) {
    *streamPtr.get() << endl << "# CUDA extern __shared__ syntax replace with HIP_DYNAMIC_SHARED() macro" << endl;
    *streamPtr.get() << sub << "transformExternShared" << " {" << endl;
    *streamPtr.get() << tab << no_warns << endl;
    *streamPtr.get() << tab << my_k << endl;
    *streamPtr.get() << tab << "$k += s/extern\\s+([\\w\\(\\)]+)?\\s*__shared__\\s+([\\w:<>\\s]+)\\s+(\\w+)\\s*\\[\\s*\\]\\s*;/HIP_DYNAMIC_SHARED($1 $2, $3)/g;" << endl;
    *streamPtr.get() << tab << "$ft{'extern_shared'} += $k;" << endl << "}" << endl;
  }

  void generateKernelLaunch(unique_ptr<ostream> &streamPtr) {
    *streamPtr.get() << endl << "# CUDA Kernel Launch Syntax" << endl << sub << "transformKernelLaunch" << " {" << endl;
    *streamPtr.get() << tab << no_warns << endl;
    *streamPtr.get() << tab << my_k << endl_2;

    string s_k = "$k += s/([:|\\w]+)\\s*";
    *streamPtr.get() << tab << "# Handle the kern<...><<<Dg, Db, Ns, S>>>() syntax with empty args:" << endl;
    *streamPtr.get() << tab << s_k << "<(.+)>\\s*<<<\\s*(.+)\\s*,\\s*(.+)\\s*,\\s*(.+)\\s*,\\s*(.+)\\s*>>>(\\s*)\\((\\s*)\\)/hipLaunchKernelGGL(HIP_KERNEL_NAME($1<$2>), dim3($3), dim3($4), $5, $6)/g;" << endl;
    *streamPtr.get() << tab << "# Handle the kern<<<Dg, Db, Ns, S>>>() syntax with empty args:" << endl;
    *streamPtr.get() << tab << s_k << "<<<\\s*(.+)\\s*,\\s*(.+)\\s*,\\s*(.+)\\s*,\\s*(.+)\\s*>>>(\\s*)\\((\\s*)\\)/hipLaunchKernelGGL($1, dim3($2), dim3($3), $4, $5)/g;" << endl_2;

    *streamPtr.get() << tab << "# Handle the kern<...><<<Dg, Db, Ns, S>>>(...) syntax with non-empty args:" << endl;
    *streamPtr.get() << tab << s_k << "<(.+)>\\s*<<<\\s*(.+)\\s*,\\s*(.+)\\s*,\\s*(.+)\\s*,\\s*(.+)\\s*>>>(\\s*)\\(/hipLaunchKernelGGL(HIP_KERNEL_NAME($1<$2>), dim3($3), dim3($4), $5, $6, /g;" << endl;
    *streamPtr.get() << tab << "# Handle the kern<<<Dg, Db, Ns, S>>>(...) syntax with non-empty args:" << endl;
    *streamPtr.get() << tab << s_k << "<<<\\s*(.+)\\s*,\\s*(.+)\\s*,\\s*(.+)\\s*,\\s*(.+)\\s*>>>(\\s*)\\(/hipLaunchKernelGGL($1, dim3($2), dim3($3), $4, $5, /g;" << endl_2;

    *streamPtr.get() << tab << "# Handle the kern<...><<<Dg, Db, Ns>>>() syntax with empty args:" << endl;
    *streamPtr.get() << tab << s_k << "<(.+)>\\s*<<<\\s*(.+)\\s*,\\s*(.+)\\s*,\\s*(.+)\\s*>>>(\\s*)\\((\\s*)\\)/hipLaunchKernelGGL(HIP_KERNEL_NAME($1<$2>), dim3($3), dim3($4), $5, 0)/g;" << endl;
    *streamPtr.get() << tab << "# Handle the kern<<<Dg, Db, Ns>>>() syntax with empty args:" << endl;
    *streamPtr.get() << tab << s_k << "<<<\\s*(.+)\\s*,\\s*(.+)\\s*,\\s*(.+)\\s*>>>(\\s*)\\((\\s*)\\)/hipLaunchKernelGGL($1, dim3($2), dim3($3), $4, 0)/g;" << endl_2;

    *streamPtr.get() << tab << "# Handle the kern<...><<Dg, Db, Ns>>>(...) syntax with non-empty args:" << endl;
    *streamPtr.get() << tab << s_k << "<(.+)>\\s*<<<\\s*(.+)\\s*,\\s*(.+)\\s*,\\s*(.+)\\s*>>>(\\s*)\\(/hipLaunchKernelGGL(HIP_KERNEL_NAME($1<$2>), dim3($3), dim3($4), $5, 0, /g;" << endl;
    *streamPtr.get() << tab << "# Handle the kern<<<Dg, Db, Ns>>>(...) syntax with non-empty args:" << endl;
    *streamPtr.get() << tab << s_k << "<<<\\s*(.+)\\s*,\\s*(.+)\\s*,\\s*(.+)\\s*>>>(\\s*)\\(/hipLaunchKernelGGL($1, dim3($2), dim3($3), $4, 0, /g;" << endl_2;

    *streamPtr.get() << tab << "# Handle the kern<...><<<Dg, Db>>>() syntax with empty args:" << endl;
    *streamPtr.get() << tab << s_k << "<(.+)>\\s*<<<\\s*(.+)\\s*,\\s*(.+)\\s*>>>(\\s*)\\((\\s*)\\)/hipLaunchKernelGGL(HIP_KERNEL_NAME($1<$2>), dim3($3), dim3($4), 0, 0)/g;" << endl;
    *streamPtr.get() << tab << "# Handle the kern<<<Dg, Db>>>() syntax with empty args:" << endl;
    *streamPtr.get() << tab << s_k << "<<<\\s*(.+)\\s*,\\s*(.+)\\s*>>>(\\s*)\\((\\s*)\\)/hipLaunchKernelGGL($1, dim3($2), dim3($3), 0, 0)/g;" << endl_2;

    *streamPtr.get() << tab << "# Handle the kern<...><<<Dg, Db>>>(...) syntax with non-empty args:" << endl;
    *streamPtr.get() << tab << s_k << "<(.+)>\\s*<<<\\s*(.+)\\s*,\\s*(.+)\\s*>>>(\\s*)\\(/hipLaunchKernelGGL(HIP_KERNEL_NAME($1<$2>), dim3($3), dim3($4), 0, 0, /g;" << endl;
    *streamPtr.get() << tab << "# Handle the kern<<<Dg, Db>>>(...) syntax with non-empty args:" << endl;
    *streamPtr.get() << tab << s_k << "<<<\\s*(.+)\\s*,\\s*(.+)\\s*>>>(\\s*)\\(/hipLaunchKernelGGL($1, dim3($2), dim3($3), 0, 0, /g;" << endl_2;

    *streamPtr.get() << tab << "if ($k) {" << endl;
    *streamPtr.get() << tab_2 << "$ft{'kernel_launch'} += $k;" << endl;
    *streamPtr.get() << tab_2 << "$Tkernels{$1}++;" << endl_tab << "}" << endl << "}" << endl;
  }

  void generateCubNamespace(unique_ptr<ostream> &streamPtr) {
    *streamPtr.get() << endl << sub << "transformCubNamespace" << " {" << endl_tab << my_k << endl;
    *streamPtr.get() << tab << "$k += s/using\\s*namespace\\s*cub/using namespace hipcub/g;" << endl;
    *streamPtr.get() << tab << "$k += s/\\bcub::\\b/hipcub::/g;" << endl << tab << return_k << "}" << endl;
  }

  void generateHostFunctions(unique_ptr<ostream> &streamPtr) {
    *streamPtr.get() << endl << sub << "transformHostFunctions" << " {" << endl_tab << my_k << endl;
    set<string> &funcSet = DeviceSymbolFunctions0;
    const string s0 = "$k += s/(?<!\\/\\/ CHECK: )($func)\\s*\\(\\s*([^,]+)\\s*,/$func\\(";
    const string s1 = "$k += s/(?<!\\/\\/ CHECK: )($func)\\s*\\(\\s*([^,]+)\\s*,\\s*([^,\\)]+)\\s*(,\\s*|\\))\\s*/$func\\($2, ";
    for (int i = 0; i < 4; ++i) {
      *streamPtr.get() << tab + foreach_func;
      switch (i) {
      case 1:  funcSet = DeviceSymbolFunctions1; break;
      case 2:  funcSet = ReinterpretFunctions0; break;
      case 3:  funcSet = ReinterpretFunctions1; break;
      default: funcSet = DeviceSymbolFunctions0;
      }
      unsigned int count = 0;
      for (auto &f : funcSet) {
        const auto found = CUDA_RUNTIME_FUNCTION_MAP.find(f);
        if (found != CUDA_RUNTIME_FUNCTION_MAP.end()) {
          *streamPtr.get() << (count ? ",\n" : "") << tab_2 << "\"" << found->second.hipName.str() << "\"";
          count++;
        }
      }
      *streamPtr.get() << endl_tab << ")" << endl_tab << "{" << endl_tab_2;
      switch (i) {
      case 0:
      default: *streamPtr.get() << s0 << sHIP_SYMBOL << "\\($2\\),/g" << endl; break;
      case 1:  *streamPtr.get() << s1 << sHIP_SYMBOL << "\\($3\\)$4/g;" << endl; break;
      case 2:  *streamPtr.get() << s0 << s_reinterpret_cast << "\\($2\\),/g" << endl; break;
      case 3:  *streamPtr.get() << s1 << s_reinterpret_cast << "\\($3\\)$4/g;" << endl; break;
      }
      *streamPtr.get() << tab << "}" << endl;
    }
    *streamPtr.get() << tab << return_k << "}" << endl;
  }

  void generateDeprecatedAndUnsupportedFunctions(unique_ptr<ostream> &streamPtr) {
    stringstream sDeprecated, sUnsupported, sCommon;
    sCommon << tab << my << "$line_num = shift;" << endl;
    sCommon << tab << my_k << "\n" << tab << foreach_func;
    sDeprecated << endl << sub << "warnDeprecatedFunctions" << " {" << endl << sCommon.str();
    sUnsupported << endl << sub << "warnUnsupportedFunctions" << " {" << endl << sCommon.str();
    unsigned int countDeprecated = 0, countUnsupported = 0;
    for (auto ma = CUDA_RENAMES_MAP().rbegin(); ma != CUDA_RENAMES_MAP().rend(); ++ma) {
        if (Statistics::isDeprecated(ma->second)) {
            sDeprecated << (countDeprecated ? ",\n" : "") << tab_2 << "\"" << ma->first.str() << "\"";
            countDeprecated++;
        }
        if (Statistics::isUnsupported(ma->second)) {
            sUnsupported << (countUnsupported ? ",\n" : "") << tab_2 << "\"" << ma->first.str() << "\"";
            countUnsupported++;
        }
    }
    sCommon.str(std::string());
    sCommon << endl_tab << ")" << endl;
    sCommon << tab << "{" << endl;
    sCommon << tab_2 << my << "$mt = m/($func)/g;" << endl;
    sCommon << tab_2 << "if ($mt) {" << endl;
    sCommon << tab_3 << "$k += $mt;" << endl;
    sDeprecated << sCommon.str();
    sUnsupported << sCommon.str();
    sCommon.str(std::string());
    sCommon << tab_2 << "}\n" << tab << "}\n" << tab << return_k << "}" << endl;
    sDeprecated << tab_3 << print << "\"  "  << warning << "deprecated identifier \\\"$func\\\": $_\\n\";" << endl << sCommon.str();
    sUnsupported << tab_3 << print << "\"  "  << warning << "unsupported identifier \\\"$func\\\": $_\\n\";" << endl << sCommon.str();
    *streamPtr.get() << sDeprecated.str();
    *streamPtr.get() << sUnsupported.str();
  }

  void generateDeviceFunctions(unique_ptr<ostream> &streamPtr) {
    unsigned int countUnsupported = 0;
    unsigned int countSupported = 0;
    stringstream sSupported;
    stringstream sUnsupported;
    for (auto ma = CUDA_DEVICE_FUNC_MAP.rbegin(); ma != CUDA_DEVICE_FUNC_MAP.rend(); ++ma) {
      bool isUnsupported = Statistics::isUnsupported(ma->second);
      (isUnsupported ? sUnsupported : sSupported) << ((isUnsupported && countUnsupported) || (!isUnsupported && countSupported) ? ",\n" : "") << tab_2 << "\"" << ma->first.str() << "\"";
      if (isUnsupported) countUnsupported++;
      else countSupported++;
    }
    stringstream subCountSupported;
    stringstream subWarnUnsupported;
    stringstream subCommon;
    string sCommon = tab + my_k + "\n" + tab + foreach_func;
    subCountSupported << endl << sub << "countSupportedDeviceFunctions" << " {" << endl << (countSupported ? sCommon : tab + return_0);
    subWarnUnsupported << endl << sub << "warnUnsupportedDeviceFunctions" << " {" << endl << (countUnsupported ? tab + my + "$line_num = shift;\n" + sCommon : tab + return_0);
    if (countSupported) subCountSupported << sSupported.str() << endl_tab << ")" << endl;
    if (countUnsupported) subWarnUnsupported << sUnsupported.str() << endl_tab << ")" << endl;
    if (countSupported || countUnsupported) {
      subCommon << tab << "{" << endl;
      subCommon << tab_2 << "# match device function from the list, except those, which have a namespace prefix (aka somenamespace::umin(...));" << endl;
      subCommon << tab_2 << "# function with only global namespace qualifier '::' (aka ::umin(...)) should be treated as a device function (and warned as well as without such qualifier);" << endl;
      subCommon << tab_2 << my << "$mt_namespace = m/(\\w+)::($func)\\s*\\(\\s*.*\\s*\\)/g;" << endl;
      subCommon << tab_2 << my << "$mt = m/($func)\\s*\\(\\s*.*\\s*\\)/g;" << endl;
      subCommon << tab_2 << "if ($mt && !$mt_namespace) {" << endl;
      subCommon << tab_3 << "$k += $mt;" << endl;
    }
    if (countSupported) subCountSupported << subCommon.str();
    if (countUnsupported) {
      subWarnUnsupported << subCommon.str();
      subWarnUnsupported << tab_3 << print << "\"  " << warning << "unsupported device function \\\"$func\\\": $_\\n\";" << endl;
    }
    if (countSupported || countUnsupported) sCommon = tab_2 + "}\n" + tab + "}\n" + tab + return_k;
    if (countSupported) subCountSupported << sCommon;
    if (countUnsupported) subWarnUnsupported << sCommon;
    subCountSupported << "}" << endl;
    subWarnUnsupported << "}" << endl;
    *streamPtr.get() << subCountSupported.str();
    *streamPtr.get() << subWarnUnsupported.str();
  }

  bool generate(bool Generate) {
    if (!Generate) return true;
    string dstHipifyPerl = hipify_perl, dstHipifyPerlDir = OutputHipifyPerlDir;
    error_code EC;
    if (!dstHipifyPerlDir.empty()) {
      string sOutputHipifyPerlDirAbsPath = getAbsoluteDirectoryPath(OutputHipifyPerlDir, EC, "output " + hipify_perl);
      if (EC) return false;
      dstHipifyPerl = sOutputHipifyPerlDirAbsPath + "/" + dstHipifyPerl;
    }
    SmallString<128> tmpFile;
    EC = sys::fs::createTemporaryFile(dstHipifyPerl, hipify_perl, tmpFile);
    if (EC) {
      llvm::errs() << "\n" << sHipify << sError << EC.message() << ": " << tmpFile << "\n";
      return false;
    }
    unique_ptr<ostream> streamPtr = unique_ptr<ostream>(new ofstream(tmpFile.c_str(), ios_base::trunc));
    generateHeader(streamPtr);
    string sConv = my + "$apiCalls   = ";
    unsigned int exclude[3] = { CONV_DEVICE_FUNC, CONV_EXTERN_SHARED, CONV_KERNEL_LAUNCH };
    *streamPtr.get() << "@statNames = (";
    for (unsigned int i = 0; i < NUM_CONV_TYPES - 1; ++i) {
      *streamPtr.get() << "\"" << counterNames[i] << "\", ";
      if (any_of(exclude, exclude + 3, [&i](unsigned int x) { return x == i; })) continue;
      sConv += "$ft{'" + string(counterNames[i]) + "'}" + (i < NUM_CONV_TYPES - 2 ? " + " : ";");
    }
    if (sConv.back() == ' ') sConv = sConv.substr(0, sConv.size() - 3) + ";";
    *streamPtr.get() << "\"" << counterNames[NUM_CONV_TYPES - 1] << "\");" << endl;
    generateStatFunctions(streamPtr);
    generateSimpleSubstitutions(streamPtr);
    generateExternShared(streamPtr);
    generateKernelLaunch(streamPtr);
    generateCubNamespace(streamPtr);
    generateHostFunctions(streamPtr);
    generateDeviceFunctions(streamPtr);
    generateDeprecatedAndUnsupportedFunctions(streamPtr);
    *streamPtr.get() << endl << "# Count of transforms in all files" << endl;
    *streamPtr.get() << my << "%tt;" << endl;
    *streamPtr.get() << "clearStats(\\%tt, \\@statNames);" << endl;
    *streamPtr.get() << "$Twarnings = 0;" << endl;
    *streamPtr.get() << "$TlineCount = 0;" << endl;
    *streamPtr.get() << my << "%TwarningTags;" << endl;
    *streamPtr.get() << my << "$fileCount = @ARGV;" << endl_2;
    *streamPtr.get() << while_ << "(@ARGV) {" << endl;
    *streamPtr.get() << tab << "$fileName=shift (@ARGV);" << endl;
    *streamPtr.get() << tab << "my $direxclude = 0;" << endl;
    *streamPtr.get() << tab << "$fileDir=dirname($fileName);" << endl;
    *streamPtr.get() << tab <<  while_ << "(($direxclude == 0) and ($fileDir ne \".\" and $fileDir ne \"/\"))  { " << endl;
    *streamPtr.get() << tab_2 << "if ($exclude_dirhash{$fileDir}) {" << endl;
    *streamPtr.get() << tab_3 << print << "\"Skipping file: $fileName in excluded directory $fileDir \\n\";" << endl;
    *streamPtr.get() << tab_3 << "$direxclude += 1;" <<  endl ;
    *streamPtr.get() << tab_2 << "} else {" << endl;
    *streamPtr.get() << tab_3 << "$fileDir = dirname($fileDir);" << endl_tab_2 << "}" << endl_tab << "}" << endl;
    *streamPtr.get() << tab << "if ($exclude_filehash{$fileName}) { " << endl;
    *streamPtr.get() << tab_2 <<  print << "\"Skipping  excluded file: $fileName \\n\";" << endl_tab << "}" << endl;
    *streamPtr.get() << tab << unless_ << "($direxclude or $exclude_filehash{$fileName}) {" << endl;
    *streamPtr.get() << tab_2 << "if ($inplace) {" << endl;
    *streamPtr.get() << tab_3 << my << "$file_prehip = \"$fileName\" . \".prehip\";" << endl;
    *streamPtr.get() << tab_3 << my << "$infile;" << endl;
    *streamPtr.get() << tab_3 << my << "$outfile;" << endl;
    *streamPtr.get() << tab_3 << "if (-e $file_prehip) {" << endl;
    *streamPtr.get() << tab_4 << "$infile  = $file_prehip;" << endl;
    *streamPtr.get() << tab_4 << "$outfile = $fileName;" << endl;
    *streamPtr.get() << tab_3 << "} else {" << endl;
    *streamPtr.get() << tab_4 << "system (\"cp $fileName $file_prehip\");" << endl;
    *streamPtr.get() << tab_4 << "$infile = $file_prehip;" << endl;
    *streamPtr.get() << tab_4 << "$outfile = $fileName;" << endl_tab_3 << "}" << endl;
    *streamPtr.get() << tab_3 << "open(INFILE,\"<\", $infile) or die \"error: could not open $infile\";" << endl;
    *streamPtr.get() << tab_3 << "open(OUTFILE,\">\", $outfile) or die \"error: could not open $outfile\";" << endl;
    *streamPtr.get() << tab_3 << "$OUTFILE = OUTFILE;" << endl;
    *streamPtr.get() << tab_2 << "} else {" << endl;
    *streamPtr.get() << tab_3 << "open(INFILE,\"<\", $fileName) or die \"error: could not open $fileName\";" << endl;
    *streamPtr.get() << tab_3 << "$OUTFILE = STDOUT;" << endl_tab_2 << "}" << endl;
    *streamPtr.get() << tab_2 << "# Count of transforms in this file" << endl;
    *streamPtr.get() << tab_2 << "clearStats(\\%ft, \\@statNames);" << endl;
    *streamPtr.get() << tab_2 << my << "$countIncludes = 0;" << endl;
    *streamPtr.get() << tab_2 << my << "$countKeywords = 0;" << endl;
    *streamPtr.get() << tab_2 << my << "$warnings = 0;" << endl;
    *streamPtr.get() << tab_2 << my << "%warningTags;" << endl;
    *streamPtr.get() << tab_2 << my << "$lineCount = 0;" << endl;
    *streamPtr.get() << tab_2 << "undef $/;" << endl;
    *streamPtr.get() << tab_2 << "# Read whole file at once, so we can match newlines" << endl;
    *streamPtr.get() << tab_2 << while_ << "(<INFILE>) {" << endl;
    *streamPtr.get() << tab_3 << "$countKeywords += m/__global__/;" << endl;
    *streamPtr.get() << tab_3 << "$countKeywords += m/__shared__/;" << endl;
    *streamPtr.get() << tab_3 << "simpleSubstitutions();" << endl;
    *streamPtr.get() << tab_3 << "transformExternShared();" << endl;
    *streamPtr.get() << tab_3 << "transformKernelLaunch();" << endl;
    *streamPtr.get() << tab_3 << "transformCubNamespace();" << endl;
    *streamPtr.get() << tab_3 << "if ($print_stats) {" << endl;
    *streamPtr.get() << tab_4 << while_ << "(/(\\b(hip|HIP)([A-Z]|_)\\w+\\b)/g) {" << endl;
    *streamPtr.get() << tab_5 << "$convertedTags{$1}++;" << endl_tab_4 << "}" << endl_tab_3 << "}" << endl;
    *streamPtr.get() << tab_3 << my << "$hasDeviceCode = $countKeywords + $ft{'device_function'};" << endl;
    *streamPtr.get() << tab_3 << unless_ << "($quiet_warnings) {" << endl;
    *streamPtr.get() << tab_4 << "# Copy into array of lines, process line-by-line to show warnings" << endl;
    *streamPtr.get() << tab_4 << "if ($hasDeviceCode or (/\\bcu|CU/) or (/<<<.*>>>/)) {" << endl;
    *streamPtr.get() << tab_5 << my << "@lines = split /\\n/, $_;" << endl;
    *streamPtr.get() << tab_5 << "# Copy the whole file" << endl;
    *streamPtr.get() << tab_5 << my << "$tmp = $_;" << endl;
    *streamPtr.get() << tab_5 << my << "$line_num = 0;" << endl;
    *streamPtr.get() << tab_5 << foreach << "(@lines) {" << endl;
    *streamPtr.get() << tab_6 << "$line_num++;" << endl;
    *streamPtr.get() << tab_6 << "# Remove any whitelisted words" << endl;
    *streamPtr.get() << tab_6 << foreach << "$w (@whitelist) {" << endl;
    *streamPtr.get() << tab_7 << "redo if s/\\b$w\\b/ZAP/" << endl_tab_6 << "}" << endl;
    *streamPtr.get() << tab_6 << my << "$tag;" << endl;
    *streamPtr.get() << tab_6 << "if ((/(\\bcuda[A-Z]\\w+)/) or (/<<<.*>>>/)) {" << endl;
    *streamPtr.get() << tab_7 << "# Flag any remaining code that look like cuda API calls: may want to add these to hipify" << endl;
    *streamPtr.get() << tab_7 << "$tag = (defined $1) ? $1 : \"Launch\";" << endl_tab_6 << "}" << endl;
    *streamPtr.get() << tab_6 << "if (defined $tag) {" << endl;
    *streamPtr.get() << tab_7 << "$warnings++;" << endl;
    *streamPtr.get() << tab_7 << "$warningTags{$tag}++;" << endl;
    *streamPtr.get() << tab_7 << print << "\"  warning: $fileName:#$line_num : $_\\n\";" << endl_tab_6 << "}" << endl;
    *streamPtr.get() << tab_6 << "$s = warnDeprecatedFunctions($line_num);" << endl;
    *streamPtr.get() << tab_6 << "$warnings += $s;" << endl;
    *streamPtr.get() << tab_6 << "$s = warnUnsupportedFunctions($line_num);" << endl;
    *streamPtr.get() << tab_6 << "$warnings += $s;" << endl;
    *streamPtr.get() << tab_6 << "$s = warnUnsupportedDeviceFunctions($line_num);" << endl;
    *streamPtr.get() << tab_6 << "$warnings += $s;" << endl_tab_5 << "}" << endl;
    *streamPtr.get() << tab_5 << "$_ = $tmp;" << endl_tab_4 << "}" << endl_tab_3 << "}" << endl;
    *streamPtr.get() << tab_3 << "if ($hasDeviceCode > 0) {" << endl;
    *streamPtr.get() << tab_4 << "$ft{'device_function'} += countSupportedDeviceFunctions();" << endl_tab_3 << "}" << endl;
    *streamPtr.get() << tab_3 << "transformHostFunctions();" << endl;
    *streamPtr.get() << tab_3 << "# TODO: would like to move this code outside loop but it uses $_ which contains the whole file" << endl;
    *streamPtr.get() << tab_3 << unless_ << "($no_output) {" << endl;
    *streamPtr.get() << tab_4 << sConv << endl;
    *streamPtr.get() << tab_4 << my << "$kernStuff  = $hasDeviceCode + $ft{'" << counterNames[CONV_KERNEL_LAUNCH] << "'} + $ft{'" << counterNames[CONV_DEVICE_FUNC] << "'};" << endl;
    *streamPtr.get() << tab_4 << my << "$totalCalls = $apiCalls + $kernStuff;" << endl;
    *streamPtr.get() << tab_4 << "$is_dos = m/\\r\\n$/;" << endl;
    *streamPtr.get() << tab_4 << "if ($totalCalls and ($countIncludes == 0) and ($kernStuff != 0)) {" << endl;
    *streamPtr.get() << tab_5 << "# TODO: implement hipify-clang's logic with header files AMAP" << endl;
    *streamPtr.get() << tab_5 << "print $OUTFILE '#include \"hip/hip_runtime.h\"' . ($is_dos ? \"\\r\\n\" : \"\\n\");" << endl_tab_4 << "}" << endl;
    *streamPtr.get() << tab_4 << "print $OUTFILE  \"$_\";" << endl_tab_3 << "}" << endl;
    *streamPtr.get() << tab_3 << "$lineCount = $_ =~ tr/\\n//;" << endl_tab_2 << "}" << endl;
    *streamPtr.get() << tab_2 << my << "$totalConverted = totalStats(\\%ft);" << endl;
    *streamPtr.get() << tab_2 << "if (($totalConverted+$warnings) and $print_stats) {" << endl;
    *streamPtr.get() << tab_3 << "printStats(\"  info: converted\", \\@statNames, \\%ft, $warnings, $lineCount);" << endl;
    *streamPtr.get() << tab_3 << print << "\" in '$fileName'\\n\";" << endl_tab_2 << "}" << endl;
    *streamPtr.get() << tab_2 << "# Update totals for all files" << endl;
    *streamPtr.get() << tab_2 << "addStats(\\%tt, \\%ft);" << endl;
    *streamPtr.get() << tab_2 << "$Twarnings += $warnings;" << endl;
    *streamPtr.get() << tab_2 << "$TlineCount += $lineCount;" << endl;
    *streamPtr.get() << tab_2 << foreach << "$key (keys %warningTags) {" << endl;
    *streamPtr.get() << tab_3 << "$TwarningTags{$key} += $warningTags{$key};" << endl_tab_2 << "}";
    *streamPtr.get() << endl_tab << "}   # Unless filtered direcotry or file " << endl;
    *streamPtr.get() << "}" << endl;
    *streamPtr.get() << "# Print total stats for all files processed:" << endl;
    *streamPtr.get() << "if ($print_stats and ($fileCount > 1)) {" << endl;
    *streamPtr.get() << tab << print << "\"\\n\";" << endl;
    *streamPtr.get() << tab << "printStats(\"  info: TOTAL-converted\", \\@statNames, \\%tt, $Twarnings, $TlineCount);" << endl;
    *streamPtr.get() << tab << print << "\"\\n\";" << endl;
    *streamPtr.get() << tab << foreach << my << "$key (sort { $TwarningTags{$b} <=> $TwarningTags{$a} } keys %TwarningTags) {" << endl;
    *streamPtr.get() << tab_2 << printf << "\"  warning: unconverted %s : %d\\n\", $key, $TwarningTags{$key};" << endl_tab << "}" << endl;
    *streamPtr.get() << tab << my << "$kernelCnt = keys %Tkernels;" << endl;
    *streamPtr.get() << tab << printf << "\"  kernels (%d total) : \", $kernelCnt;" << endl;
    *streamPtr.get() << tab << foreach << my << "$key (sort { $Tkernels{$b} <=> $Tkernels{$a} } keys %Tkernels) {" << endl;
    *streamPtr.get() << tab_2 << printf << "\"  %s(%d)\", $key, $Tkernels{$key};" << endl_tab << "}" << endl;
    *streamPtr.get() << tab << print << "\"\\n\\n\";" << endl << "}" << endl;
    *streamPtr.get() << "if ($print_stats) {" << endl;
    *streamPtr.get() << tab << foreach << my << "$key (sort { $convertedTags{$b} <=> $convertedTags{$a} } keys %convertedTags) {" << endl;
    *streamPtr.get() << tab_2 << printf << "\"  %s %d\\n\", $key, $convertedTags{$key};" << endl_tab << "}" << endl << "}" << endl;
    streamPtr.get()->flush();
    bool ret = true;
    EC = sys::fs::copy_file(tmpFile, dstHipifyPerl);
    if (EC) {
      llvm::errs() << "\n" << sHipify << sError << EC.message() << ": while copying " << tmpFile << " to " << dstHipifyPerl << "\n";
      ret = false;
    }
    if (!SaveTemps) sys::fs::remove(tmpFile);
    return ret;
  }
}
