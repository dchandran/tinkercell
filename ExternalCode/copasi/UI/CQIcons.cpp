// Begin CVS Header
//   $Source: /fs/turing/cvs/copasi_dev/copasi/UI/CQIcons.cpp,v $
//   $Revision: 1.2 $
//   $Name: Build-33 $
//   $Author: pwilly $
//   $Date: 2010/05/30 15:23:55 $
// End CVS Header

// Copyright (C) 2010 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

#include "CQIcons.h"

static const unsigned char PauseIcon[] =
{
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10,
  0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0xf3, 0xff, 0x61, 0x00, 0x00, 0x00,
  0x31, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0x63, 0xfc, 0xff, 0xff, 0x3f,
  0x03, 0x25, 0x80, 0x89, 0x22, 0xdd, 0x83, 0xc2, 0x00, 0x16, 0x74, 0x01,
  0x46, 0x46, 0x46, 0x78, 0xa0, 0xfc, 0xff, 0xff, 0x9f, 0x11, 0x97, 0x18,
  0xd5, 0x5c, 0x30, 0x6a, 0xc0, 0x60, 0x30, 0x80, 0x71, 0x34, 0x2f, 0x30,
  0x00, 0x00, 0xa1, 0x5e, 0x0e, 0x1b, 0xc0, 0x83, 0x3e, 0xb5, 0x00, 0x00,
  0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static const unsigned char ContinueIcon[] =
{
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10,
  0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0xf3, 0xff, 0x61, 0x00, 0x00, 0x00,
  0x41, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0x63, 0x60, 0x18, 0xac, 0xe0,
  0x3f, 0xb1, 0x0a, 0x99, 0x28, 0x35, 0x04, 0x9f, 0x01, 0x44, 0x19, 0x42,
  0xc8, 0x00, 0x82, 0x86, 0x10, 0x63, 0x00, 0x5e, 0x43, 0x88, 0x35, 0x00,
  0xa7, 0x21, 0xa4, 0x18, 0x40, 0xb1, 0x0b, 0x18, 0x29, 0x31, 0x00, 0xab,
  0x66, 0x62, 0x0d, 0xc0, 0xa9, 0x99, 0x18, 0x03, 0xf0, 0x6a, 0xc6, 0x07,
  0x88, 0x4e, 0xca, 0x03, 0x0f, 0x00, 0x87, 0xd3, 0x07, 0x12, 0x18, 0xc5,
  0xb3, 0xe8, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42,
  0x60, 0x82
};

static const unsigned char StopIcon[] =
{
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10,
  0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0xf3, 0xff, 0x61, 0x00, 0x00, 0x00,
  0x2f, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0x63, 0xfc, 0xff, 0xff, 0x3f,
  0x03, 0x25, 0x80, 0x89, 0x22, 0xdd, 0x83, 0xc2, 0x00, 0x16, 0x74, 0x01,
  0x46, 0x46, 0x46, 0xbc, 0x81, 0xf2, 0xff, 0xff, 0x7f, 0x46, 0xaa, 0xba,
  0x60, 0xd4, 0x80, 0xc1, 0x60, 0x00, 0xe3, 0x68, 0x5e, 0x60, 0x00, 0x00,
  0xf2, 0xba, 0x0a, 0x1b, 0x43, 0x7a, 0xe1, 0x23, 0x00, 0x00, 0x00, 0x00,
  0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static const unsigned char SelectObjectIcon[] =
{
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10,
  0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0xf3, 0xff, 0x61, 0x00, 0x00, 0x02,
  0x51, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0x8d, 0x93, 0xdb, 0x53, 0x52,
  0x51, 0x14, 0x87, 0x7f, 0x1b, 0x37, 0x17, 0xe3, 0x2e, 0x8e, 0x98, 0x97,
  0x22, 0x01, 0x51, 0xb0, 0xcb, 0x28, 0x8c, 0x3a, 0xa6, 0x65, 0x4d, 0x33,
  0xdd, 0xa6, 0x9e, 0x7d, 0xe9, 0xef, 0xeb, 0xc5, 0xe9, 0xb1, 0x97, 0xa6,
  0x51, 0x31, 0x72, 0x72, 0x86, 0xd1, 0x20, 0x06, 0x27, 0x27, 0xec, 0x88,
  0xe0, 0x81, 0x40, 0xe0, 0x40, 0x9c, 0xa3, 0x70, 0x56, 0x0f, 0x8d, 0x34,
  0x27, 0xa3, 0xda, 0x8f, 0x7b, 0xad, 0xef, 0x9b, 0xbd, 0xd6, 0xda, 0x8b,
  0xc5, 0x62, 0x31, 0xfc, 0xcf, 0x29, 0x88, 0x97, 0x49, 0x91, 0x55, 0xd8,
  0x6c, 0x7a, 0xd8, 0x9d, 0x59, 0x76, 0x7e, 0xcf, 0xff, 0x06, 0x7d, 0xd9,
  0x1f, 0x20, 0xe1, 0xa0, 0x81, 0x7c, 0xae, 0x89, 0x62, 0x51, 0x00, 0xd3,
  0x31, 0x78, 0xbd, 0x66, 0x3c, 0x7e, 0xf6, 0x2b, 0xa7, 0xab, 0x60, 0xfd,
  0xad, 0x99, 0xd2, 0xa9, 0x1c, 0x0c, 0x06, 0x8e, 0xc9, 0x29, 0x07, 0x96,
  0xee, 0x98, 0x30, 0x7c, 0xf5, 0x98, 0x01, 0x8a, 0x26, 0xef, 0x8f, 0x82,
  0xd5, 0x97, 0x7a, 0x3a, 0x3a, 0xaa, 0xc2, 0xeb, 0xb3, 0xe0, 0xc9, 0x73,
  0x99, 0x01, 0x95, 0xae, 0xaf, 0xd4, 0x08, 0x92, 0xbb, 0x0e, 0xfa, 0xf0,
  0xfe, 0x04, 0x92, 0xd4, 0xc4, 0x74, 0xd8, 0x89, 0xc5, 0x65, 0x89, 0x75,
  0x03, 0x2f, 0x08, 0x76, 0xe3, 0x26, 0xda, 0x8c, 0x0a, 0x30, 0x1a, 0x2d,
  0x58, 0x5c, 0xee, 0xc7, 0x74, 0xb8, 0xf2, 0x4f, 0x58, 0x23, 0xd8, 0x8a,
  0x89, 0xb0, 0x5a, 0x55, 0xcc, 0x2f, 0x38, 0xe0, 0x0f, 0x5c, 0x84, 0xbd,
  0x8d, 0x0c, 0x29, 0xcd, 0x1a, 0x94, 0x96, 0x04, 0xa6, 0xd7, 0xa3, 0xe0,
  0x9a, 0x65, 0x1d, 0xc1, 0xab, 0xd5, 0x0a, 0xa9, 0x44, 0x88, 0xcc, 0x8e,
  0xc2, 0x1f, 0xa8, 0x69, 0xe0, 0xa1, 0xbd, 0x75, 0x2a, 0xa5, 0x13, 0x48,
  0x89, 0x39, 0x54, 0x65, 0x05, 0x27, 0x44, 0x60, 0x1c, 0x18, 0xf6, 0x44,
  0x69, 0xe2, 0xe1, 0x0a, 0x78, 0xee, 0x90, 0xd3, 0xe1, 0xd7, 0x1c, 0x66,
  0xc2, 0x7e, 0x4c, 0x86, 0xea, 0x1d, 0x78, 0xbc, 0x16, 0x27, 0x31, 0x16,
  0xc5, 0x5e, 0x3a, 0x8b, 0xfc, 0x99, 0x05, 0xad, 0xc1, 0x29, 0xb8, 0x6f,
  0x78, 0xe0, 0xeb, 0xb7, 0xe2, 0xb4, 0x94, 0xc1, 0x51, 0x72, 0x1b, 0x7d,
  0x87, 0x19, 0xf0, 0x5a, 0x59, 0xc4, 0x40, 0xdf, 0x19, 0x16, 0x96, 0xf4,
  0x1d, 0x78, 0xac, 0xbc, 0x45, 0xc2, 0xc6, 0x6b, 0x64, 0x04, 0xa0, 0x7d,
  0x65, 0x1e, 0x9e, 0xd0, 0x2d, 0x18, 0x02, 0x63, 0x9d, 0x78, 0x9b, 0x19,
  0xe9, 0x58, 0x4e, 0xc0, 0x5e, 0xd6, 0x81, 0xab, 0x6d, 0x05, 0x0e, 0xab,
  0xaa, 0xa9, 0xb7, 0x94, 0x58, 0xc7, 0x27, 0xc1, 0x08, 0x43, 0xe8, 0x01,
  0x46, 0xef, 0x47, 0x2e, 0xf4, 0x23, 0xba, 0x16, 0x47, 0x51, 0x54, 0xe0,
  0x33, 0xb9, 0xc0, 0x0d, 0x66, 0x2b, 0xc4, 0x92, 0xd0, 0x09, 0x06, 0x75,
  0x7b, 0x94, 0x3a, 0x29, 0x20, 0xab, 0x8b, 0x20, 0x12, 0xf0, 0x6b, 0xc0,
  0x6a, 0x2a, 0x4e, 0x3b, 0xef, 0xb6, 0x90, 0xfd, 0x9c, 0x87, 0x77, 0x2a,
  0x88, 0xc1, 0x60, 0x80, 0x71, 0xa7, 0xc3, 0x0e, 0x55, 0xd5, 0x61, 0x73,
  0xad, 0x44, 0xb7, 0xef, 0xba, 0x58, 0x93, 0xf7, 0xc3, 0x64, 0x33, 0xa3,
  0xb7, 0x95, 0x06, 0x09, 0x26, 0xd8, 0x64, 0x23, 0x49, 0x55, 0x09, 0x89,
  0x74, 0x01, 0xfb, 0xe9, 0x03, 0xc8, 0x0a, 0xc7, 0xa8, 0xef, 0x26, 0xee,
  0xbd, 0x58, 0xf9, 0x39, 0x05, 0x97, 0x5b, 0x65, 0x81, 0xe0, 0x35, 0x4a,
  0x7e, 0x14, 0x20, 0x35, 0x38, 0xcd, 0x5c, 0x77, 0xc2, 0xee, 0x7e, 0x04,
  0xa7, 0x6d, 0x03, 0x3b, 0x6f, 0x36, 0xb1, 0xad, 0xb4, 0xd1, 0x62, 0x3d,
  0xa8, 0xcb, 0x3d, 0x20, 0xdb, 0x08, 0x7c, 0xb3, 0x73, 0x08, 0x3f, 0x9d,
  0xeb, 0x94, 0xc5, 0xce, 0xb7, 0x31, 0xbe, 0xfd, 0x9d, 0x12, 0xc9, 0x6f,
  0x28, 0x8a, 0x15, 0xb0, 0xb3, 0x32, 0x5a, 0x8d, 0x3a, 0xea, 0xd5, 0x53,
  0xf0, 0x1e, 0x60, 0x68, 0xc4, 0x82, 0xf1, 0x80, 0x1b, 0x13, 0xa1, 0x11,
  0xb8, 0xc7, 0x5d, 0x9a, 0x9e, 0xb0, 0xdf, 0xd7, 0x39, 0x9f, 0x33, 0x52,
  0xa3, 0xde, 0x00, 0xb5, 0x75, 0xe0, 0x9c, 0xa3, 0xf7, 0x92, 0x01, 0x83,
  0xc3, 0x72, 0xd7, 0x5f, 0xf9, 0x03, 0x51, 0x05, 0xf9, 0xb7, 0x17, 0xf5,
  0x66, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42,
  0x60, 0x82
};

static const unsigned char AddIcon[] =
{
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10,
  0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0xf3, 0xff, 0x61, 0x00, 0x00, 0x01,
  0x62, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0x95, 0x93, 0x4f, 0x4a, 0x03,
  0x31, 0x18, 0xc5, 0x7f, 0xc9, 0xa4, 0x2d, 0xa5, 0x82, 0x68, 0x41, 0x41,
  0x10, 0xbb, 0x70, 0x27, 0x5e, 0x20, 0x22, 0x82, 0xe8, 0x4e, 0xef, 0x50,
  0x2f, 0xe1, 0xce, 0x85, 0x2b, 0xaf, 0xa0, 0x2b, 0x11, 0xa1, 0xa8, 0x0b,
  0x17, 0x9e, 0xa1, 0xce, 0xc2, 0xb5, 0x3d, 0x80, 0x16, 0xb5, 0x05, 0xc5,
  0x96, 0x8a, 0x58, 0x6d, 0xf3, 0xb9, 0x98, 0x99, 0xfe, 0x9b, 0xa9, 0xd4,
  0x07, 0x21, 0xf9, 0xc2, 0xcb, 0xcb, 0x9b, 0x37, 0x5f, 0x94, 0xef, 0xfb,
  0x8c, 0xc2, 0x9e, 0x5a, 0xe1, 0x1b, 0x70, 0xe1, 0x86, 0x06, 0xd2, 0xe0,
  0xef, 0xf9, 0x6a, 0x94, 0x6b, 0x62, 0xa7, 0x01, 0xda, 0x20, 0x67, 0xc3,
  0x5b, 0xaa, 0x98, 0xc8, 0x1c, 0x23, 0x20, 0xc1, 0x94, 0xbf, 0x5e, 0xe2,
  0xb3, 0xf9, 0xca, 0xe6, 0x72, 0x01, 0x54, 0xe5, 0x1f, 0x02, 0x21, 0x72,
  0xc6, 0xa1, 0x53, 0x42, 0xd6, 0x13, 0x88, 0x99, 0x0f, 0xa0, 0xff, 0x12,
  0x98, 0x04, 0x26, 0x16, 0x18, 0x40, 0x7b, 0x98, 0x24, 0x08, 0xfc, 0x80,
  0x3d, 0xb6, 0x12, 0x7d, 0x5e, 0x14, 0xac, 0x49, 0x0a, 0xac, 0x67, 0x4f,
  0x85, 0xbe, 0xbb, 0xc2, 0xee, 0xbe, 0xe5, 0x66, 0x75, 0xf8, 0x8f, 0xa9,
  0x22, 0x98, 0x48, 0x51, 0x9f, 0xe7, 0x99, 0x1a, 0x48, 0x44, 0x21, 0x64,
  0x4d, 0x8b, 0x54, 0x26, 0xc3, 0x6d, 0xad, 0x8e, 0x7b, 0xae, 0x31, 0x7d,
  0x3f, 0x8b, 0x84, 0x61, 0x78, 0x06, 0xf0, 0xde, 0xfa, 0x21, 0xaa, 0xf6,
  0x07, 0xba, 0x2b, 0xa1, 0x65, 0x70, 0xce, 0xf1, 0xa5, 0x35, 0x28, 0x85,
  0x48, 0x70, 0x2c, 0x1a, 0x81, 0xab, 0x60, 0x65, 0xf0, 0x82, 0x7a, 0x65,
  0x61, 0x8e, 0x9c, 0x71, 0x20, 0xd0, 0xe9, 0x38, 0x76, 0xd6, 0xd6, 0x29,
  0x95, 0xcb, 0xbc, 0x37, 0x1a, 0x6c, 0x14, 0x0a, 0x3c, 0xb6, 0x9a, 0x68,
  0xdd, 0x57, 0xd0, 0x9e, 0xc6, 0xd7, 0x55, 0x0c, 0x75, 0x50, 0xdb, 0x80,
  0xae, 0xd2, 0xbb, 0x7e, 0x1e, 0xb6, 0xac, 0xc2, 0x49, 0x50, 0x8a, 0x16,
  0xee, 0x2e, 0x9e, 0xe0, 0x65, 0xc0, 0x82, 0x02, 0x3c, 0x30, 0xfe, 0x41,
  0xbc, 0x3d, 0xed, 0x89, 0x95, 0xa3, 0x99, 0x4b, 0x4a, 0xb2, 0xd8, 0x67,
  0x1b, 0xf0, 0x0f, 0xe3, 0xdc, 0xe4, 0x3e, 0x90, 0xa1, 0x29, 0xa1, 0x98,
  0x50, 0x60, 0x12, 0x24, 0x0b, 0x84, 0xc1, 0x9a, 0xb4, 0x46, 0xa7, 0x14,
  0x5e, 0x5a, 0x8d, 0xed, 0xd9, 0xe4, 0xb7, 0xd0, 0x0b, 0xf6, 0x01, 0x14,
  0x5c, 0x49, 0x65, 0xec, 0xab, 0xf9, 0x05, 0x38, 0x3f, 0x6e, 0x47, 0xd8,
  0x50, 0x6b, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae,
  0x42, 0x60, 0x82
};

static const unsigned char DeleteIcon[] =
{
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10,
  0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0xf3, 0xff, 0x61, 0x00, 0x00, 0x01,
  0x1c, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0xa5, 0x92, 0xb1, 0x6a, 0xc3,
  0x30, 0x14, 0x45, 0x8f, 0x8c, 0xdb, 0x82, 0xe9, 0xda, 0x31, 0x05, 0x8f,
  0x9d, 0x8d, 0x28, 0xf8, 0x1b, 0xf2, 0x0b, 0x86, 0x40, 0x86, 0x2e, 0xdd,
  0xfa, 0x1d, 0xdd, 0x32, 0x76, 0x08, 0xf8, 0x0f, 0x42, 0xc1, 0x7f, 0xf0,
  0x20, 0x94, 0xcc, 0xde, 0xb3, 0x06, 0x3a, 0x15, 0x43, 0xda, 0x50, 0x75,
  0x50, 0xac, 0xca, 0x96, 0xd2, 0x52, 0xfa, 0x40, 0x3c, 0xa4, 0xfb, 0xce,
  0xc5, 0x3c, 0x5f, 0x25, 0x22, 0xfc, 0xa7, 0x12, 0xff, 0x92, 0xce, 0x4b,
  0x93, 0xce, 0x4b, 0x73, 0x6a, 0x38, 0xa6, 0x27, 0xbe, 0xa8, 0x6b, 0x83,
  0xae, 0x0d, 0x31, 0x93, 0x53, 0x7a, 0xe2, 0x8b, 0x34, 0x39, 0x34, 0x79,
  0x30, 0xf4, 0x93, 0x9e, 0x02, 0xac, 0x5b, 0xd0, 0x4d, 0x6e, 0xa7, 0x3f,
  0x0e, 0xb0, 0x9a, 0xa0, 0x6b, 0xc3, 0x4b, 0xa5, 0x0c, 0x80, 0xae, 0x0d,
  0xac, 0x26, 0x56, 0x3f, 0x4b, 0xa1, 0xc9, 0x59, 0xb7, 0x50, 0x00, 0xaa,
  0x5f, 0xe2, 0xa6, 0x2c, 0xcd, 0xfd, 0xc3, 0xb5, 0x1d, 0xda, 0x77, 0xb6,
  0xcf, 0x76, 0xb6, 0x2f, 0xaf, 0x6c, 0xbf, 0xc8, 0x00, 0x58, 0x3c, 0x6e,
  0x29, 0x44, 0x14, 0xbe, 0x81, 0x33, 0x99, 0x1d, 0x87, 0xdf, 0xbb, 0xe1,
  0x12, 0xce, 0x8f, 0xf0, 0x72, 0xe7, 0xe0, 0xc0, 0xc0, 0x99, 0x4c, 0x2f,
  0xc7, 0x3b, 0xb4, 0xf0, 0xf3, 0xdb, 0x00, 0x76, 0x3b, 0xf0, 0xeb, 0xf6,
  0x06, 0x78, 0xed, 0xc6, 0xcf, 0x4e, 0x3b, 0x8c, 0xde, 0x82, 0x1c, 0xe8,
  0xda, 0xc0, 0xfe, 0x33, 0x7a, 0x62, 0xbf, 0x38, 0xc8, 0x01, 0xd5, 0xe0,
  0x0b, 0x87, 0x55, 0xa9, 0x5f, 0x72, 0x30, 0x82, 0x17, 0x1b, 0x7b, 0x5c,
  0x65, 0xa1, 0x49, 0x02, 0x36, 0x07, 0x0e, 0xce, 0xbe, 0xe1, 0x42, 0x44,
  0x15, 0x22, 0x2a, 0x30, 0xb9, 0x53, 0x96, 0xe9, 0x0d, 0x0a, 0x11, 0xf5,
  0xd4, 0x86, 0x70, 0xcf, 0x8c, 0x4d, 0x7c, 0x3d, 0xc8, 0x41, 0x0f, 0xc4,
  0x56, 0x10, 0xd3, 0x83, 0x1c, 0xfc, 0xb5, 0xbe, 0x00, 0x40, 0xce, 0xa4,
  0xe3, 0x14, 0x6f, 0x5e, 0xa8, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e,
  0x44, 0xae, 0x42, 0x60, 0x82
};

static const unsigned char ViewIcon[] =
{
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10,
  0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0xf3, 0xff, 0x61, 0x00, 0x00, 0x01,
  0xa2, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0x9d, 0x93, 0x3b, 0x4b, 0x2b,
  0x41, 0x18, 0x86, 0x9f, 0x59, 0x76, 0x73, 0x8f, 0xd7, 0x28, 0x18, 0x05,
  0x51, 0x04, 0xe1, 0x80, 0x85, 0x1a, 0x2c, 0x16, 0x0b, 0x7f, 0x80, 0x17,
  0x30, 0x9d, 0x20, 0x08, 0x01, 0xb5, 0xb0, 0xb0, 0xf1, 0x57, 0x08, 0x36,
  0xda, 0x9c, 0x9c, 0x26, 0xa4, 0xb4, 0xb3, 0xb6, 0x10, 0x9b, 0xa9, 0x5c,
  0x41, 0x44, 0x10, 0x21, 0x95, 0x57, 0x10, 0xa3, 0x32, 0x12, 0x93, 0x78,
  0x59, 0x8b, 0x4d, 0x96, 0x4d, 0x34, 0xca, 0xf1, 0xab, 0x66, 0xbe, 0x99,
  0xe7, 0xfd, 0xde, 0x99, 0xf9, 0x46, 0x48, 0x29, 0xf9, 0x2e, 0xcc, 0x8c,
  0xcf, 0xf6, 0xce, 0xe5, 0x42, 0x59, 0x78, 0xe7, 0xfa, 0x4f, 0xe0, 0xf2,
  0xfc, 0x08, 0x00, 0xf7, 0xa5, 0x6a, 0xfe, 0xd0, 0xf6, 0x0a, 0x89, 0xaf,
  0x1c, 0x98, 0x19, 0x9f, 0xbd, 0xb5, 0x3a, 0xcc, 0x40, 0x54, 0xc3, 0xd0,
  0x20, 0x7b, 0x06, 0x47, 0x79, 0x50, 0x65, 0x78, 0xb5, 0x41, 0x17, 0x90,
  0xdb, 0xb3, 0x90, 0x0b, 0x65, 0xd1, 0xd0, 0x41, 0x8b, 0xa1, 0xd1, 0xea,
  0x83, 0xb1, 0x4e, 0xb8, 0x29, 0xc0, 0x76, 0xce, 0x81, 0xfd, 0x75, 0xc4,
  0x27, 0x81, 0x6a, 0xf5, 0x7c, 0x19, 0xce, 0x0b, 0xf0, 0x70, 0x09, 0x39,
  0x05, 0x83, 0xed, 0xd0, 0x11, 0x82, 0x80, 0x0e, 0xc5, 0x57, 0x20, 0x39,
  0x8a, 0x99, 0xb1, 0xec, 0x86, 0x0e, 0x2e, 0x9f, 0xe1, 0xb6, 0x04, 0x85,
  0x17, 0x67, 0x6c, 0x76, 0x43, 0x57, 0x18, 0x82, 0x06, 0x3c, 0xbf, 0x38,
  0x7b, 0x76, 0xbf, 0xbc, 0x44, 0xcb, 0xa2, 0x2b, 0x3c, 0xc2, 0xfd, 0x1b,
  0x5c, 0x3c, 0xc1, 0x5d, 0x11, 0xfa, 0x5a, 0xa0, 0xb7, 0x09, 0x7a, 0x22,
  0x10, 0xf5, 0x83, 0x2a, 0x81, 0x2a, 0xbc, 0x83, 0x65, 0x35, 0x7e, 0x85,
  0xb6, 0x00, 0xbc, 0x01, 0xcd, 0x41, 0x88, 0x87, 0xa1, 0x33, 0x04, 0xb1,
  0x10, 0xc4, 0x23, 0x70, 0xf5, 0x04, 0xb1, 0x90, 0x06, 0x80, 0x56, 0x0f,
  0xca, 0xcd, 0x21, 0x91, 0x9c, 0xfd, 0x87, 0xdf, 0x80, 0x9e, 0x28, 0x74,
  0x47, 0xa0, 0x3d, 0x08, 0x61, 0xc3, 0x59, 0xcf, 0x17, 0x9d, 0x7b, 0x18,
  0x9f, 0x48, 0x23, 0x37, 0x87, 0x6a, 0x5f, 0xe1, 0x38, 0x63, 0x56, 0x9a,
  0xe6, 0x2f, 0xa9, 0xde, 0x8a, 0xe0, 0x03, 0x3c, 0xbe, 0x3b, 0x50, 0xab,
  0x0e, 0x7f, 0x02, 0xb5, 0x05, 0x75, 0x2f, 0x3c, 0x99, 0x9c, 0x26, 0xde,
  0x3f, 0xc3, 0xd4, 0x69, 0x0a, 0x91, 0x70, 0xf2, 0xf6, 0xc1, 0x62, 0x0d,
  0x20, 0x12, 0x69, 0xd7, 0xa9, 0x2b, 0x50, 0x85, 0x63, 0x31, 0xc1, 0xf5,
  0x69, 0x0a, 0xa5, 0x60, 0x2e, 0xb0, 0xc4, 0xca, 0xba, 0x14, 0x22, 0x91,
  0xae, 0x6d, 0xe5, 0x0a, 0xe8, 0x3a, 0xf0, 0xc2, 0x77, 0x57, 0x3b, 0x28,
  0x05, 0x4a, 0x35, 0x06, 0xea, 0x43, 0x03, 0x3e, 0xc1, 0x1b, 0x59, 0x58,
  0x59, 0x97, 0xdf, 0x82, 0xae, 0xc0, 0xfe, 0x09, 0xbf, 0x86, 0xa1, 0xf2,
  0x99, 0xb6, 0xd6, 0x4c, 0xf7, 0x9c, 0xff, 0x03, 0x03, 0x7c, 0x00, 0x79,
  0x53, 0xa2, 0x24, 0xa1, 0xd8, 0xe8, 0xa2, 0x00, 0x00, 0x00, 0x00, 0x49,
  0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static const unsigned char EditIcon[] =
{
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10,
  0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0xf3, 0xff, 0x61, 0x00, 0x00, 0x01,
  0x5c, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0x9d, 0xd0, 0xb1, 0x4b, 0x02,
  0x61, 0x18, 0xc7, 0xf1, 0xef, 0xab, 0x62, 0xa2, 0x83, 0xf4, 0x07, 0x24,
  0x95, 0x25, 0x14, 0x1a, 0x08, 0xde, 0xe2, 0xd8, 0x5e, 0x41, 0x82, 0xb7,
  0x44, 0xe1, 0xa6, 0xd1, 0xd2, 0xdc, 0x1c, 0x24, 0x04, 0x81, 0x41, 0xa5,
  0x43, 0x4a, 0x24, 0xe1, 0xd0, 0xdc, 0xe2, 0x24, 0x74, 0x53, 0x92, 0x0e,
  0x81, 0x38, 0x36, 0xb4, 0x99, 0x8b, 0x0e, 0x92, 0xf0, 0x36, 0x9c, 0x77,
  0x1a, 0x69, 0x77, 0xf4, 0x4c, 0xef, 0xf0, 0x7e, 0x7e, 0x3c, 0xcf, 0x4f,
  0x68, 0x9a, 0x86, 0xdd, 0x71, 0xa5, 0xe2, 0xd2, 0x78, 0x0f, 0x8b, 0x9a,
  0x00, 0x70, 0xd8, 0xc5, 0x6a, 0xc5, 0x23, 0x1d, 0x65, 0x89, 0xe2, 0x07,
  0xa5, 0x2c, 0xcd, 0x30, 0x5b, 0x01, 0x6a, 0xc5, 0x23, 0xcf, 0xd2, 0x21,
  0x0a, 0xf9, 0x3a, 0xf5, 0x2b, 0x09, 0x87, 0x02, 0xc5, 0xaf, 0x6f, 0x64,
  0x19, 0x90, 0x1c, 0xe1, 0xc8, 0x92, 0x0f, 0xbf, 0xba, 0x4c, 0x21, 0x5f,
  0xe7, 0x26, 0x65, 0x5e, 0x82, 0xcb, 0x0a, 0x67, 0xd3, 0x21, 0xa2, 0x41,
  0x1f, 0xc5, 0x5a, 0x17, 0xad, 0xdd, 0xa1, 0x11, 0x8b, 0xc0, 0x8b, 0x1e,
  0x32, 0xcc, 0x08, 0xc4, 0xac, 0x12, 0x27, 0x71, 0xa9, 0xd6, 0xe5, 0xb9,
  0xdd, 0xa1, 0xe1, 0x0c, 0xd0, 0x77, 0xb8, 0xf5, 0xc3, 0xaf, 0xef, 0xd0,
  0x72, 0xab, 0x62, 0xea, 0x06, 0xd3, 0x70, 0xd3, 0x19, 0xa0, 0xef, 0x72,
  0xeb, 0x1f, 0x46, 0x18, 0xa6, 0x94, 0x38, 0x0b, 0xf7, 0x0c, 0x5c, 0xb8,
  0x35, 0xf1, 0xaf, 0x0e, 0x92, 0x15, 0x9f, 0xcc, 0xa6, 0x57, 0x88, 0x06,
  0xbd, 0xb3, 0xf1, 0xc5, 0x9a, 0x98, 0x34, 0x66, 0x80, 0x5a, 0x99, 0x97,
  0xd9, 0x74, 0x80, 0x70, 0xd0, 0x6b, 0x16, 0x66, 0x85, 0xcd, 0x13, 0xea,
  0xf9, 0xb8, 0x7c, 0xdf, 0x3b, 0x21, 0x1c, 0xf4, 0x72, 0x5f, 0xeb, 0x52,
  0x6d, 0xf5, 0x68, 0xd8, 0xc0, 0x3f, 0x3b, 0x88, 0x3d, 0xb0, 0xd1, 0x4c,
  0x50, 0x6d, 0xf5, 0x78, 0x9d, 0x5b, 0xd0, 0xdb, 0xb6, 0xc0, 0x00, 0xe2,
  0xf2, 0x00, 0x79, 0x54, 0x1a, 0xc0, 0xd7, 0x27, 0x0c, 0x3e, 0xa0, 0x75,
  0x8a, 0x68, 0x3c, 0xea, 0xd1, 0x16, 0x78, 0xdc, 0x81, 0x81, 0x07, 0x1f,
  0xb0, 0xb8, 0x8b, 0x24, 0x81, 0xc8, 0xec, 0xa0, 0xe5, 0xfe, 0xc6, 0x00,
  0x62, 0x5d, 0xd9, 0x94, 0x00, 0x6f, 0x4f, 0xc7, 0x00, 0xec, 0x6f, 0x6f,
  0x01, 0x90, 0x39, 0xd7, 0x2c, 0xf1, 0x78, 0x83, 0x7f, 0x40, 0x63, 0xbe,
  0x01, 0xfa, 0x49, 0xa9, 0x02, 0xe4, 0x53, 0x67, 0x1d, 0x00, 0x00, 0x00,
  0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static const unsigned char SaveIcon[] =
{
  0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
  0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10,
  0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0xf3, 0xff, 0x61, 0x00, 0x00, 0x01,
  0xd7, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0x8d, 0x93, 0xbf, 0x6b, 0x93,
  0x51, 0x14, 0x86, 0x9f, 0x73, 0x73, 0xbf, 0xef, 0xcb, 0x8f, 0xa2, 0xd2,
  0x45, 0xb4, 0x36, 0x9b, 0x2e, 0x15, 0x04, 0x5d, 0xd2, 0x16, 0x3a, 0x54,
  0x1c, 0x5c, 0xdc, 0x8b, 0x8b, 0x83, 0x8a, 0x8b, 0x9b, 0x92, 0x51, 0xc5,
  0x49, 0x10, 0x17, 0xc1, 0x45, 0xfc, 0x13, 0x9c, 0x14, 0x3a, 0x74, 0xd5,
  0x50, 0x1a, 0x4b, 0x44, 0x70, 0x10, 0xa1, 0x56, 0xc1, 0x58, 0x15, 0x0b,
  0x36, 0xd5, 0xe4, 0x4b, 0x72, 0xef, 0x71, 0xb8, 0x29, 0x89, 0x41, 0x93,
  0xde, 0xed, 0x70, 0xcf, 0x79, 0xcf, 0xf3, 0x9e, 0x7b, 0xae, 0x70, 0xe5,
  0x95, 0xd2, 0xee, 0xf0, 0xf5, 0x68, 0x89, 0xdf, 0xca, 0xbe, 0x4e, 0x5e,
  0xe0, 0x70, 0x7d, 0x15, 0xe2, 0x08, 0x8b, 0xf3, 0x2c, 0x3f, 0x28, 0x91,
  0x9f, 0x84, 0xc6, 0xfe, 0xea, 0xc9, 0x03, 0xcb, 0xdb, 0x25, 0xce, 0xdf,
  0xac, 0x22, 0x5c, 0x5d, 0x57, 0x9e, 0xad, 0xa0, 0xf5, 0x32, 0x9d, 0x6e,
  0x07, 0x41, 0x46, 0x16, 0x2b, 0x4a, 0x64, 0x23, 0xe4, 0xc8, 0x3d, 0xb8,
  0x70, 0x0e, 0x8b, 0x73, 0x9c, 0x79, 0x5c, 0x06, 0xa0, 0xba, 0x16, 0x91,
  0x31, 0xa3, 0xbb, 0x3b, 0x0f, 0x73, 0x73, 0x70, 0xfd, 0x79, 0x99, 0x87,
  0x8f, 0xaa, 0x58, 0x54, 0xf1, 0x9d, 0x70, 0x79, 0xea, 0x74, 0x0b, 0x19,
  0x0d, 0x80, 0x2a, 0x40, 0x96, 0x77, 0x3f, 0x42, 0x60, 0x07, 0x2f, 0xdf,
  0xd4, 0xb2, 0x98, 0x31, 0x04, 0xde, 0xc3, 0xec, 0x2c, 0x6c, 0xfc, 0x04,
  0x84, 0xbf, 0x05, 0x8e, 0xcf, 0xec, 0x62, 0xc6, 0x20, 0x78, 0x55, 0x60,
  0x82, 0x8d, 0xc6, 0x80, 0xc0, 0x5e, 0xd3, 0x1c, 0x3a, 0x66, 0x84, 0x61,
  0x88, 0x00, 0xbe, 0x19, 0x62, 0x0b, 0x90, 0xb3, 0x90, 0x7d, 0x02, 0xe9,
  0xb6, 0x05, 0x9b, 0xe1, 0xbf, 0x3e, 0xbc, 0x07, 0x85, 0xe9, 0xad, 0xb7,
  0x30, 0x7d, 0x02, 0x44, 0x7a, 0x04, 0x02, 0x05, 0x0b, 0xa9, 0xb6, 0xe1,
  0xc5, 0x3a, 0xd4, 0x6a, 0x20, 0x43, 0x22, 0xaa, 0x70, 0x76, 0x01, 0xba,
  0x8e, 0xcb, 0x4b, 0x27, 0xb9, 0xf5, 0xba, 0xd9, 0x27, 0x80, 0xe0, 0x87,
  0x9d, 0x26, 0xfa, 0x74, 0x11, 0x58, 0xfc, 0x27, 0x80, 0x5c, 0xaa, 0x82,
  0x08, 0x33, 0xc5, 0x02, 0xac, 0x7e, 0x1b, 0x12, 0x00, 0x48, 0xd3, 0x90,
  0x38, 0x75, 0x1f, 0x2e, 0x2e, 0xf1, 0xe9, 0xda, 0x14, 0xf5, 0xef, 0x10,
  0x27, 0x9e, 0xc9, 0x83, 0xfd, 0x3d, 0xf7, 0x0a, 0xa4, 0xed, 0x9e, 0x05,
  0x63, 0x02, 0xad, 0x00, 0x69, 0x2b, 0x64, 0x14, 0x2c, 0xd0, 0x66, 0xb7,
  0x01, 0xb9, 0xbc, 0x27, 0x4a, 0x1c, 0x49, 0x76, 0xe8, 0xa3, 0xb4, 0x42,
  0xae, 0x91, 0x8f, 0x1f, 0xd8, 0xac, 0xbe, 0xc7, 0xd8, 0x40, 0xa0, 0x4d,
  0xa0, 0xdd, 0x85, 0x9d, 0x5f, 0x44, 0x31, 0x44, 0x89, 0x23, 0x8a, 0x1d,
  0x49, 0xd6, 0x0f, 0x78, 0xe9, 0x09, 0x88, 0x60, 0x5f, 0xde, 0x39, 0x26,
  0x0b, 0xb7, 0x6b, 0x9a, 0x74, 0x2c, 0xa8, 0xa7, 0xd1, 0x80, 0xcd, 0x95,
  0x1b, 0x24, 0x31, 0xb8, 0x0c, 0x4c, 0xc4, 0x86, 0x24, 0x81, 0x43, 0x07,
  0x3c, 0x64, 0x32, 0x60, 0x94, 0x38, 0x01, 0x5c, 0x17, 0x8c, 0x41, 0x2a,
  0x95, 0x0a, 0x00, 0xf3, 0x77, 0xbf, 0xa8, 0x7e, 0xde, 0x02, 0xa7, 0x7b,
  0xfb, 0xda, 0x9b, 0xbe, 0xf4, 0xbb, 0x46, 0xbd, 0x97, 0x71, 0x3e, 0x2c,
  0x4f, 0xb1, 0xc8, 0x1f, 0x9e, 0x09, 0xa8, 0x8c, 0xc0, 0x44, 0x13, 0x6b,
  0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

// static
QPixmap CQIcons::getIcon(const CQIcons::IconID & id)
{
  QImage img;

  switch (id)
    {
      case Pause:
        img.loadFromData(PauseIcon, sizeof(PauseIcon), "PNG");
        break;

      case Continue:
        img.loadFromData(ContinueIcon, sizeof(ContinueIcon), "PNG");
        break;

      case Stop:
        img.loadFromData(StopIcon, sizeof(StopIcon), "PNG");
        break;

      case SelectObject:
        img.loadFromData(SelectObjectIcon, sizeof(SelectObjectIcon), "PNG");
        break;

      case Add:
        img.loadFromData(AddIcon, sizeof(AddIcon), "PNG");
        break;

      case Delete:
        img.loadFromData(DeleteIcon, sizeof(DeleteIcon), "PNG");
        break;

      case View:
        img.loadFromData(ViewIcon, sizeof(ViewIcon), "PNG");
        break;

      case Edit:
        img.loadFromData(EditIcon, sizeof(EditIcon), "PNG");
        break;

      case Save:
        img.loadFromData(SaveIcon, sizeof(SaveIcon), "PNG");
        break;

      default:
        break;
    } // switch

  return QPixmap::fromImage(img);
} // icon