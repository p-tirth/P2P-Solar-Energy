import "../styles/globals.css";
import "@rainbow-me/rainbowkit/styles.css";

import { getDefaultWallets, RainbowKitProvider } from "@rainbow-me/rainbowkit";
import { configureChains, createConfig, useAccount, WagmiConfig } from "wagmi";

import {
	mainnet,
	polygon,
	optimism,
	arbitrum,
	goerli,
	polygonMumbai,
	optimismGoerli,
	arbitrumGoerli,
	polygonZkEvm,
	polygonZkEvmTestnet,
} from "wagmi/chains";


import { alchemyProvider } from "wagmi/providers/alchemy";
import { publicProvider } from "wagmi/providers/public";
import MainLayout from "../layout/mainLayout";
import { useRouter } from "next/router";

const { chains, publicClient, webSocketPublicClient } = configureChains(
	[
		mainnet,
		goerli,
		polygon,
		polygonMumbai,
		optimism,
		optimismGoerli,
		arbitrum,
		arbitrumGoerli,
		polygonZkEvm,
		polygonZkEvmTestnet
	],
	[alchemyProvider({ apiKey: "wO_V1V152EdDVRKlVWwWZx27g0aPy6mN" }), publicProvider()]
);

const { connectors } = getDefaultWallets({
	appName: "My Alchemy DApp",
	projectId:"78d5c71bae81f55e8fca1ec18a9e525a",
	chains,
});

const config = createConfig({
	autoConnect: true,
	connectors,
	publicClient, webSocketPublicClient
});

export { WagmiConfig, RainbowKitProvider };

function MyApp({ Component, pageProps }) {
	return (
		<WagmiConfig config={config}>
			<RainbowKitProvider
				modalSize="compact"
				initialChain="goerli"
				chains={chains}
			>
				<MainLayout>
					<Component {...pageProps} />
				</MainLayout>
			</RainbowKitProvider>
		</WagmiConfig>
	);
}

export default MyApp;
