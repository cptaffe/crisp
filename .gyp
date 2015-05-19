{
	'target_defaults': {
		'xcode_settings': {
			'OTHER_CFLAGS': [
				'--std=c++11',
				'-g',
			],
		},
		'cflags': [
			'--std=c++11',
			'-g',
		],
	},
	'targets': [
		{
			'target_name': 'libcrisp',
			'type': 'static_library',
			'dependencies': [],
			'sources': [
				'lexer.cc',
				'scanner.cc',
				'token.cc',
				'tree.cc',
			],
			'include_dirs': [],
		},
		{
			'target_name': 'test',
			'type': 'executable',
			'dependencies': [
				'libcrisp',
			],
			'defines': [],
			'include_dirs': [],
			'sources': [
				'main.cc',
			],
		},
	],
}
