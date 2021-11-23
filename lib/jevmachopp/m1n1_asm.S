	.arch armv8-a
	.text

	.global	printf
	.type	printf, %function
printf:
	b debug_printf
	.size	printf, .-printf
