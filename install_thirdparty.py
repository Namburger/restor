import argparse
import subprocess
import glob
import re
import os

parser = argparse.ArgumentParser(description='Installing ThirdParty')

parser.add_argument('--all', help='install all dependencies', default=True)
parser.add_argument('--reinstall_all',
                    help='re-install all dependencies', default=False)
parser.add_argument(
    '--reinstall', help='reinstall one library', type=str, default="")


def install_all_deps():
    subprocess.call("./thirdparty/rules/cmake.sh")
    for rule in glob.glob("./thirdparty/rules/*.sh"):
        subprocess.call("./" + rule, shell=True)


def reinstall_all_deps():
    to_remove = str("./thirdparty/all/")
    if os.path.exists(to_remove):
        subprocess.call(str("rm -r " + to_remove), shell=True)
    install_all_deps()


def reinstall_one_lib(lib):
    all_rules = glob.glob("./thirdparty/rules/*.sh")
    names = []
    for rule in all_rules:
        names.append(re.search('%s(.*)%s' %
                               ('./thirdparty/rules/', '.sh'), rule).group(1))
    if lib not in names:
        print('Error no rules found for ' + lib)
    else:
        to_remove = str("./thirdparty/all/" + lib)
        if os.path.exists(to_remove):
            subprocess.call(str("rm -r " + to_remove), shell=True)
        subprocess.call("./thirdparty/rules/" + lib + ".sh", shell=True)


args = parser.parse_args()

if args.reinstall != "":
    reinstall_one_lib(args.reinstall)
elif args.reinstall_all:
    reinstall_all_deps()
else:
    install_all_deps()
