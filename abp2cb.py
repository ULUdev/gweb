#!/usr/bin/env python

# This script parses the ABP file format
# (https://help.eyeo.com/en/adblockplus/how-to-write-filters) to webkits content
# blockers (https://webkit.org/blog/3476/content-blockers-first-look/)

import json
import argparse
import sys, errno

def parse_input(_input: str) -> list:
    content: str = ""
    if _input == "-":
            while True:
                try:
                    content.append(input())
                except:
                    break
    else:
        with open(_input, "r") as file:
            content = file.read()

    out = []

    # domain_exceptions = []
    # for exc in [i[3:] if i.startswith('@@') else pass for i in line]:
    #     # TODO: handle exceptions
    #     pass

    for line in content.splitlines():
        action = {"type": "block"}
        trigger = {}
        # line is a comment or an exception or file version
        if line.startswith('!') or line.startswith('['):
            continue

        if line.startswith('@@'):
            trigger

        # unify css blocking
        if "#@" in line:
            # line contains exception rule
            continue

        line.replace('#?', '#')
        line.replace('#$', '#')


        if "##" in line:
            filter_str, opts = line.split('##', 1)
            action["type"] = "css-display-none"
            if "-abp" in opts:
                # TODO: parse -abp options to css 
                continue
            action["selector"] = opts
        else:
            filter_str = line.split('$')[0]
            if len(line.split('$')) > 1:
                opts = line.split('$')[1]
            else:
                opts = ""

        # regexify the content
        filter_str.replace('.', '\\.')
        filter_str.replace('^', '.')
        filter_str.replace('||', '^')
        filter_str.replace('|', '$')

        trigger["url-filter"] = filter_str
        out.append({"trigger": trigger, "action": action})
        
    return out

def write_output(file_name: str, json: dict):
    if file_name == "-":
        print(json)
    else:
        with open(file_name, "w+") as file:
            file.write(json.dumps(json, indent=4).replace('\'', '\"'))

__version__ = "1.3.4"

def main(argv: list[str]):
    try:
        argument_parser = argparse.ArgumentParser(description="Convert an ABP file to safaris Content Blockers.", prog="abp2cb.py")
        argument_parser.add_argument('-i', '--input', help="The input file to use. Set this to '-' or omit it to read from stdin", default="-")
        argument_parser.add_argument('-v', '--version', action="version", version=f"%(prog)s {__version__}")
        argument_parser.add_argument('-o', '--output', help="The output file to use. Default is '-' meaning stdout", default="-")
        args = argument_parser.parse_args(argv)
        input_file = args.input
        output_file = args.output
        write_output(output_file, parse_input(input_file))
    except IOError as e:
        if e.errno == errno.EPIPE:
            pass

if __name__ == "__main__":
    main(sys.argv[1:])
