#!/usr/bin/env python3

import os
import io
import sys
import re
import xml.etree.ElementTree as ET

# on msys, use crlf output
nl = None
if sys.platform == 'msys':
    nl = "\r\n"

# Get the file, relative to this script's location (same directory)
# that way we're not sensitive to CWD
pathname = os.path.abspath(os.path.dirname(sys.argv[0])) + os.path.sep

# open the file for write
f = open(pathname + 'vk_dispatch_defs.h', mode='w', newline = nl)

# open XML registry
registry = ET.parse(pathname + 'vk.xml').getroot()

# f.write the file, starting with a template header
f.write('''
/******************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Baldur Karlsson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

/******************************************************************************
 * Generated from Khronos's vk.xml:
'''.lstrip())

def prefix_star(line):
    if line == '':
        return ' *'
    else:
        return ' * ' + line

# Print the first two comments with the license
for comment in registry.findall('comment')[0:2]:
    f.write("\n".join([prefix_star(line.strip()) for line in comment.text.split('\n')]))

f.write('''
 ******************************************************************************/

// This file is autogenerated with gen_dispatch_table.py - any changes will be overwritten next time
// that script is run.
// $ ./gen_spirv_code.py

#pragma once

#include "official/vulkan.h"

// this file is autogenerated, so don't worry about clang-format issues
// clang-format off
'''.lstrip())

platform_defines = {}

# Cache the platform defines that protect each platform name
for plat in registry.findall('platforms/platform'):
    platform_defines[plat.attrib['name']] = plat.attrib['protect']

# Process all commands and categorise into instance or device
commands = {}
INSTANCE_CMD = 1
DEVICE_CMD = 2

# Some special cases we manually set
commands['vkCreateInstance'] = INSTANCE_CMD
commands['vkEnumerateInstanceVersion'] = INSTANCE_CMD
commands['vkEnumerateInstanceLayerProperties'] = INSTANCE_CMD
commands['vkEnumerateInstanceExtensionProperties'] = INSTANCE_CMD

import xml

for cmd in registry.findall('commands/command'):

    if 'alias' in cmd.attrib:
        name = cmd.attrib['name']
        alias = cmd.attrib['alias']
        if alias not in commands:
            raise ValueError('alias {} of {} defined, but {} is unknown'.format(name, alias, alias))

        commands[name] = commands[alias]
        continue

    name = cmd.find('proto/name').text
    if name in commands:
        continue

    first_param_type = cmd.find('param/type').text

    if first_param_type == 'VkInstance' or first_param_type == 'VkPhysicalDevice':
        commands[name] = INSTANCE_CMD
    elif first_param_type == 'VkDevice' or first_param_type == 'VkQueue' or first_param_type == 'VkCommandBuffer':
        commands[name] = DEVICE_CMD
    else:
        raise ValueError('type {} of first parameter to {} is unexpected'.format(first_param_type, name))

inst_commands = ""
dev_commands = ""
processed_commands = [] # some commands come from multiple extensions. Include them only in the first

def process_feature(root, name):
    global inst_commands, dev_commands, processed_commands

    inst = ""
    dev = ""

    for req in root.findall('require'):
        for cmd in req.findall('command'):
            function = cmd.attrib['name']

            if function in processed_commands:
                continue

            processed_commands.append(function)

            if function not in commands:
                raise ValueError('command {} referenced by {} is unknown'.format(function, name))

            table = commands[function]

            if table == INSTANCE_CMD:
                inst += '\n  PFN_{} {};'.format(function, function[2:])
            elif table == DEVICE_CMD:
                dev += '\n  PFN_{} {};'.format(function, function[2:])
            else:
                raise ValueError('command {} has unknown table type {}'.format(function, table))

    if 'platform' in root.attrib:
        if inst != "":
            inst = '\n#ifdef {plat}{inst}\n#endif // {plat}'.format(plat = platform_defines[root.attrib['platform']], inst = inst)
        if dev != "":
            dev = '\n#ifdef {plat}{dev}\n#endif // {plat}'.format(plat = platform_defines[root.attrib['platform']], dev = dev)

    if inst != "":
        inst_commands += "  // {name}{inst}\n\n".format(**locals())
    if dev != "":
        dev_commands += "  // {name}{dev}\n\n".format(**locals())

# Look at all features
for feat in registry.findall('feature'):
    # Only process vulkan features
    if 'api' in feat.attrib and feat.attrib['api'] == 'vulkan':
        process_feature(feat, feat.attrib['comment'])

# And all extensions (with KHR extensions sorted to the front)
def ext_sort(ext):
    if 'KHR' in ext.attrib['name']:
        return int(ext.attrib['number'])
    return 10000000 + int(ext.attrib['number'])


for ext in sorted(registry.findall('extensions/extension'), key=ext_sort):
    # Only process vulkan extensions
    if 'supported' in ext.attrib and ext.attrib['supported'] == 'vulkan':

        process_feature(ext, ext.attrib['name'])

inst_commands = inst_commands.strip()
dev_commands = dev_commands.strip()

f.write('''
struct VkInstDispatchTable
{{
  {inst_commands}
}};

struct VkDevDispatchTable
{{
  {dev_commands}

  // for consistency with macros, we declare the CreateDevice pointer here
  // even though it won't actually ever get used and is on the instance dispatch chain
  PFN_vkCreateDevice CreateDevice;
}};
'''.format(**locals()))
