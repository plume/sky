package bin

import (
	"bytes"
	"compress/gzip"
	"io"
)

// query_tmpl returns raw, uncompressed file data.
func query_tmpl() []byte {
	gz, err := gzip.NewReader(bytes.NewBuffer([]byte{
		0x1f, 0x8b, 0x08, 0x00, 0x00, 0x09, 0x6e, 0x88, 0x00, 0xff, 0x64, 0x8d,
		0x31, 0x6e, 0x85, 0x30, 0x0c, 0x40, 0xf7, 0x9c, 0xc2, 0xfa, 0xcb, 0xa7,
		0x1d, 0x3a, 0xb4, 0x23, 0xea, 0x49, 0x10, 0x42, 0x56, 0x30, 0xc1, 0x6a,
		0x08, 0xd4, 0x71, 0x90, 0x28, 0xe2, 0xee, 0x75, 0xd5, 0x09, 0x98, 0xf2,
		0xf2, 0xe2, 0x3c, 0xc7, 0xd9, 0x63, 0x84, 0x61, 0x60, 0xf8, 0x04, 0xa1,
		0xef, 0xc2, 0x42, 0xd5, 0xd3, 0xae, 0xcf, 0x17, 0xe7, 0xec, 0x78, 0xf3,
		0x3d, 0x0d, 0x55, 0xd3, 0x38, 0xdd, 0x16, 0x32, 0x84, 0xac, 0x52, 0xbc,
		0x42, 0xfe, 0xda, 0x3a, 0x43, 0x4e, 0xa1, 0x53, 0xd8, 0x1d, 0x00, 0x27,
		0xfd, 0x78, 0x37, 0x8e, 0x94, 0x82, 0x8e, 0xb5, 0x19, 0x3f, 0xa2, 0xc0,
		0x6b, 0x8f, 0x8a, 0xb5, 0x3b, 0x4e, 0x1f, 0x6a, 0xd7, 0xb6, 0x96, 0xdf,
		0x77, 0xa5, 0x69, 0x89, 0xa8, 0x04, 0x0f, 0x5a, 0x29, 0xe9, 0xe3, 0x38,
		0x4e, 0xd2, 0x17, 0xc9, 0xb3, 0xfc, 0xd9, 0x93, 0x1e, 0x39, 0xeb, 0x1c,
		0x04, 0xa7, 0xeb, 0x3c, 0xae, 0x24, 0x18, 0xe8, 0xaa, 0x7b, 0x9b, 0xe7,
		0xe4, 0x6f, 0xf9, 0xa2, 0x1c, 0x6f, 0x71, 0x4e, 0xac, 0x8c, 0x91, 0x7f,
		0x6e, 0x19, 0x0c, 0x41, 0x28, 0x18, 0x5e, 0x1f, 0x26, 0x92, 0xff, 0xa5,
		0xbf, 0x01, 0x00, 0x00, 0xff, 0xff, 0x0c, 0x00, 0x2a, 0x8f, 0x4c, 0x01,
		0x00, 0x00,
	}))

	if err != nil {
		panic("Decompression failed: " + err.Error())
	}

	var b bytes.Buffer
	io.Copy(&b, gz)
	gz.Close()

	return b.Bytes()
}

func init() {
	go_bindata["/query.tmpl"] = query_tmpl
}
