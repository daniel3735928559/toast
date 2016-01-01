;-------------------------------- FASM VESA INFORMATION BLOCK -----------------------------------

VESA_Info:		
VESAInfo_Signature			times 4 db 0	;resb	4      ; VBE Signature
VESAInfo_Version				dw 0				;resw	1      ; VBE Version
VESAInfo_OEMStringPtr		dd 0				;resd	1      ; VbeFarPtr to OEM String
VESAInfo_Capabilities		times 4 db 0	;resb	4      ; Capabilities of graphics controller
VESAInfo_VideoModePtr		dd 0				;resd	1      ; VbeFarPtr to VideoModeList
VESAInfo_TotalMemory			dw 0				;resw	1      ; Number of 64kb memory blocks
VESAInfo_OEMSoftwareRev		dw 0				;resw	1      ; VBE implementation Software revision
VESAInfo_OEMVendorNamePtr	dd 0				;resd	1      ; VbeFarPtr to Vendor Name String
VESAInfo_OEMProductNamePtr	dd 0				;resd	1      ; VbeFarPtr to Product Name String
VESAInfo_OEMProductRevPtr	dd 0				;resd	1      ; VbeFarPtr to Product Revision String
VESAInfo_Reserved				times 222 db 0	;resb	222    ; Reserved for VBE implementation scratch area
VESAInfo_OEMData				times 256 db 0	;resb	256    ; Data Area for OEM Strings

;============================== VESA MODE INFORMATION ===========================================

Mode_Info:		
ModeInfo_ModeAttributes			dw 0			 ;resw	1      ; mode attributes
ModeInfo_WinAAttributes			db 0			 ;resb	1      ; window A attributes
ModeInfo_WinBAttributes			db 0			 ;resb	1      ; window B attributes
ModeInfo_WinGranularity			dw 0			 ;resw	1      ; window granularity
ModeInfo_WinSize					dw 0			 ;resw	1      ; window size
ModeInfo_WinASegment				dw 0			 ;resw	1      ; window A start segment
ModeInfo_WinBSegment				dw 0			 ;resw	1      ; window B start segment
ModeInfo_WinFuncPtr				dd 0			 ;resd	1      ; real mode pointer to window function
ModeInfo_BytesPerScanLine		dw 0			 ;resw	1      ; bytes per scan line
ModeInfo_XResolution				dw 0			 ;resw	1      ; horizontal resolution in pixels or characters
ModeInfo_YResolution				dw 0			 ;resw	1      ; vertical resolution in pixels or characters
ModeInfo_XCharSize				db 0			 ;resb	1      ; character cell width in pixels
ModeInfo_YCharSize				db 0			 ;resb	1      ; character cell height in pixels
ModeInfo_NumberOfPlanes			db 0			 ;resb	1      ; number of memory planes
ModeInfo_BitsPerPixel			db 0			 ;resb	1      ; bits per pixel
ModeInfo_NumberOfBanks			db 0			 ;resb	1      ; number of banks
ModeInfo_MemoryModel				db 0			 ;resb	1      ; memory model type
ModeInfo_BankSize					db 0			 ;resb	1      ; bank size in KB
ModeInfo_NumberOfImagePages	db 0			 ;resb	1      ; number of images
ModeInfo_Reserved_page			db 0			 ;resb	1      ; reserved for page function
ModeInfo_RedMaskSize				db 0			 ;resb	1      ; size of direct color red mask in bits
ModeInfo_RedMaskPos				db 0			 ;resb	1      ; bit position of lsb of red mask
ModeInfo_GreenMaskSize			db 0			 ;resb	1      ; size of direct color green mask in bits
ModeInfo_GreenMaskPos			db 0			 ;resb	1      ; bit position of lsb of green mask
ModeInfo_BlueMaskSize			db 0			 ;resb	1      ; size of direct color blue mask in bits
ModeInfo_BlueMaskPos				db 0			 ;resb	1      ; bit position of lsb of blue mask
ModeInfo_ReservedMaskSize		db 0			 ;resb	1      ; size of direct color reserved mask in bits
ModeInfo_ReservedMaskPos		db 0			 ;resb	1      ; bit position of lsb of reserved mask
ModeInfo_DirectColorModeInfo	db 0			 ;resb	1      ; direct color mode attributes
; VBE 2.0 extensions
ModeInfo_PhysBasePtr				dd 0			 ;resd	1      ; *physical address for flat memory frame buffer*
ModeInfo_OffScreenMemOffset	dd 0			 ;resd	1      ; Reserved - always set to 0
ModeInfo_OffScreenMemSize		dw 0			 ;resw	1      ; Reserved - always set to 0
; VBE 3.0 extensions
ModeInfo_LinBytesPerScanLine	dw 0			 ;resw	1      ; bytes per scan line for linear modes
ModeInfo_BnkNumberOfPages		db 0			 ;resb	1      ; number of images for banked modes
ModeInfo_LinNumberOfPages		db 0			 ;resb	1      ; number of images for linear modes
ModeInfo_LinRedMaskSize			db 0			 ;resb	1      ; size of direct color red mask (linear modes)
ModeInfo_LinRedFieldPos			db 0			 ;resb	1      ; bit position of lsb of red mask (linear modes)
ModeInfo_LinGreenMaskSize		db 0			 ;resb	1      ; size of direct color green mask (linear modes)
ModeInfo_LinGreenFieldPos		db 0			 ;resb	1      ; bit position of lsb of green mask (linear modes)
ModeInfo_LinBlueMaskSize		db 0			 ;resb	1      ; size of direct color blue mask (linear modes)
ModeInfo_LinBlueFieldPos		db 0			 ;resb	1      ; bit position of lsb of blue mask (linear modes)
ModeInfo_LinRsvdMaskSize		db 0			 ;resb	1      ; size of direct color reserved mask (linear modes)
ModeInfo_LinRsvdFieldPos		db 0			 ;resb	1      ; bit position of lsb of reserved mask (linear modes)
ModeInfo_MaxPixelClock			dd 0			 ;resd	1      ; maximum pixel clock (in Hz) for graphics mode
; Reserved
ModeInfo_Reserved					times 190 db 0		;resb	190    ; remainder of ModeInfoBlock
;======================================= START OF PROGRAM  ======================================


