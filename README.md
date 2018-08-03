VegaGraphicsFixup
===================

An open source kernel extension providing simple initialisation of Vega adapters.

#### Installation
- Must install Lilu.kext (make sure Release/Debug config match Lilu extension type)

#### Features
- Fixes for 5K display
- Allow screen hotplug

#### How it works
 - Remove board-id in AppleGraphicsDevicePolicy, identical to putting the following patch in CLOVER's config.plist KextsToPatch section
 			<dict>
				<key>Comment</key>
				<string>AppleGraphicsDevicePolicy board-id -&gt; board-ix</string>
				<key>Disabled</key>
				<false/>
				<key>Find</key>
				<data>
				Ym9hcmQtaWQ=
				</data>
				<key>InfoPlistPatch</key>
				<false/>
				<key>Name</key>
				<string>AppleGraphicsDevicePolicy</string>
				<key>Replace</key>
				<data>
				Ym9hcmQtaXg=
				</data>
        </dict>


#### Credits
- [Apple](https://www.apple.com) for macOS  
- [vit9696](https://github.com/vit9696) for [Lilu.kext](https://github.com/vit9696/Lilu) & for zero-length string comparison patch (AppleGraphicsDevicePolicy.kext )
- [Pike R. Alpha](https://github.com/Piker-Alpha) for board-id patch (AppleGraphicsDevicePolicy.kext)
- [FredWst](http://www.insanelymac.com/forum/user/509660-fredwst/)
- [igork](https://applelife.ru/members/igork.564) for adding properties IOVARendererID & IOVARendererSubID in nvAcceleratorParent::SetAccelProperties
- [mologie](https://github.com/mologie/NVWebDriverLibValFix) for creating NVWebDriverLibValFix.kext which forces macOS to recognize NVIDIA's web drivers as platform binaries
- [lvs1974](https://applelife.ru/members/lvs1974.53809) for writing the software and maintaining it
