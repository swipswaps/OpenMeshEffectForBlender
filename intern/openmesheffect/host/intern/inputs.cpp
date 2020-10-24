/*
 * Copyright 2019 Elie Michel
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>

#include "util/memory_util.h"

#include "inputs.h"

// // OfxInputStruct

OfxMeshInputStruct::OfxMeshInputStruct()
{
  properties.context = PROP_CTX_INPUT;
  host = NULL;
}

OfxMeshInputStruct::~OfxMeshInputStruct()
{
}

void OfxMeshInputStruct::deep_copy_from(const OfxMeshInputStruct &other)
{
  this->name = other.name;  // weak pointer?
  this->properties.deep_copy_from(other.properties);
  this->mesh.deep_copy_from(other.mesh);
  this->host = other.host;  // not deep copied, as this is a weak pointer
}

// // OfxInputSetStruct

OfxMeshInputSetStruct::OfxMeshInputSetStruct()
{
  num_inputs = 0;
  inputs = NULL;
  host = NULL;
}

OfxMeshInputSetStruct::~OfxMeshInputSetStruct()
{
  for (int i = 0; i < num_inputs; ++i) {
    delete inputs[i];
  }
  num_inputs = 0;
  if (NULL != inputs) {
    free_array(inputs);
    inputs = NULL;
  }
}

int OfxMeshInputSetStruct::find(const char *input) const
{
  for (int i = 0 ; i < this->num_inputs ; ++i) {
    if (0 == strcmp(this->inputs[i]->name, input)) {
      return i;
    }
  }
  return -1;
}

void OfxMeshInputSetStruct::append(int count)
{
  int old_num_input = this->num_inputs;
  OfxMeshInputStruct **old_inputs = this->inputs;
  this->num_inputs += count;
  this->inputs = (OfxMeshInputStruct **)malloc_array(
      sizeof(OfxMeshInputStruct *), this->num_inputs, "inputs");
  for (int i = 0; i < this->num_inputs; ++i) {
    OfxMeshInputStruct *input;
    if (i < old_num_input) {
      input = old_inputs[i];
    } else {
      input = new OfxMeshInputStruct();
      input->host = this->host;
    }
    this->inputs[i] = input;
  }
  if (NULL != old_inputs) {
    free_array(old_inputs);
  }
}

int OfxMeshInputSetStruct::ensure(const char *input)
{
  int i = find(input);
  if (i == -1) {
    append(1);
    i = this->num_inputs - 1;
    this->inputs[i]->name = input;
  }
  return i;
}

void OfxMeshInputSetStruct::deep_copy_from(const OfxMeshInputSetStruct &other)
{
  append(other.num_inputs);
  for (int i = 0 ; i < this->num_inputs ; ++i) {
    this->inputs[i]->deep_copy_from(*other.inputs[i]);
  }
  this->host = other.host; // not deep copied, as this is a weak pointer
}

