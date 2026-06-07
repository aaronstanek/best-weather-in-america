<script lang="ts">
	import Head from '$lib/head.svelte';
	import Anchor from '$lib/links/anchor.svelte';
	import Link from '$lib/links/link.svelte';
	import Hidden from '$lib/hidden.svelte';

	import { unicode } from '$lib/unicode';

	import { onDestroy, onMount } from 'svelte';

	let isPageDestroyed = $state(false);

	type Condition = {
		unixTimeSeconds?: number;
		easternTimeString?: string;
		city?: string;
		state?: string;
		temperature?: string;
		humidity?: string;
		wind?: string;
		rain?: string;
	};

	let knownConditions: Condition[] = $state([]);
	let pageClockUnixTimeSeconds = $state(0);
	let validConditions = $derived(
		knownConditions.filter((x) =>
			x.unixTimeSeconds === undefined
				? true
				: x.unixTimeSeconds <= pageClockUnixTimeSeconds &&
					x.unixTimeSeconds > pageClockUnixTimeSeconds - 86400
		)
	);
	let currentCondition = $derived(validConditions.length > 0 ? validConditions[0] : {});

	const loadConditions = async () => {
		const target = ""; // API ENDPOINT URL REDACTED
		let response: Response;
		try {
			response = await fetch(target);
		} catch (e) {
			return 'error-network';
		}
		if (!response.ok) {
			return 'error-server';
		}
		let responseBody: any;
		try {
			responseBody = await response.json();
		} catch (e) {
			if (e instanceof SyntaxError) {
				return 'error-server';
			}
			return 'error-network';
		}
		if (!Array.isArray(responseBody)) {
			return 'error-server';
		}
		const newConditions: Condition[] = [];
		for (let inputCondition of responseBody) {
			if (typeof inputCondition !== 'object' || inputCondition === null) {
				return 'error-server';
			}
			const unixTimeSeconds = inputCondition.unixTimeSeconds;
			const easternTimeString = inputCondition.easternTimeString;
			const city = inputCondition.city;
			const state = inputCondition.state;
			const temperature = inputCondition.temperature;
			const humidity = inputCondition.humidity;
			const wind = inputCondition.wind;
			const rain = inputCondition.rain;
			newConditions.push({
				unixTimeSeconds:
					typeof unixTimeSeconds === 'number' && isFinite(unixTimeSeconds)
						? unixTimeSeconds
						: undefined,
				easternTimeString:
					typeof easternTimeString === 'string' && easternTimeString.length <= 8
						? easternTimeString
						: undefined,
				city: typeof city === 'string' && city.length <= 100 ? city : undefined,
				state: typeof state === 'string' && state.length === 2 ? state : undefined,
				temperature:
					typeof temperature === 'string' && temperature.length <= 8 ? temperature : undefined,
				humidity: typeof humidity === 'string' && humidity.length <= 8 ? humidity : undefined,
				wind: typeof wind === 'string' && wind.length <= 8 ? wind : undefined,
				rain: typeof rain === 'string' && rain.length <= 16 ? rain : undefined
			});
		}
		knownConditions = newConditions;
		return 'ok';
	};

	let updaterTimeout: number | undefined = $state(undefined);
	let nextLoadTimeMs: number = $state(0);

	const scheduleUpdater = (nowMs: number) => {
		const thisHourInMs = Math.floor(nowMs / 3600000) * 3600000;
		pageClockUnixTimeSeconds = Math.round(thisHourInMs / 1000);
		const delayForNextLoadMs = nextLoadTimeMs - nowMs;
		const delayForNextHourMs = thisHourInMs + 3600000 - nowMs;
		updaterTimeout = setTimeout(updater, Math.min(60000, delayForNextLoadMs, delayForNextHourMs));
	};

	const updater = async () => {
		updaterTimeout = undefined;
		if (Date.now() < nextLoadTimeMs) {
			scheduleUpdater(Date.now());
			return;
		}
		let backoff = 100;
		while (true) {
			if (isPageDestroyed) {
				return;
			}
			const result = await loadConditions();
			if (result === 'ok') {
				break;
			}
			if (result === 'error-server') {
				console.error('server error');
				return;
			}
			await new Promise((resolve) => setTimeout(resolve, backoff));
			backoff = Math.min(backoff * 2, 60000);
		}
		const nowMs = Date.now();
		const thisHourInMs = Math.floor((nowMs + 120000) / 3600000) * 3600000;
		const offset = Math.floor(Math.random() * 105000) + 14000;
		nextLoadTimeMs = thisHourInMs + 3600000 - offset;
		scheduleUpdater(nowMs);
	};

	onMount(() => {
		if (new URL(window.location.href).hostname === '' /* HOSTNAME REDACTED */) {
			updater();
		}
	});

	onDestroy(() => {
		isPageDestroyed = true;
		if (updaterTimeout !== undefined) {
			clearTimeout(updaterTimeout);
			updaterTimeout = undefined;
		}
	});

	let mapIframe: HTMLIFrameElement | undefined = $state(undefined);

	$effect(() => {
		if (mapIframe === undefined) {
			return;
		}
		if (currentCondition.city === undefined || currentCondition.state === undefined) {
			mapIframe.src = 'about:blank';
			return;
		}
		const prefix = ""; // API ENDPOINT URL REDACTED
		const qComponent = encodeURIComponent(
			`${currentCondition.city}, ${currentCondition.state}, United States`
		);
		mapIframe.src = `${prefix}&q=${qComponent}&zoom=5`;
	});
</script>

<Head
	title="Best Weather in America"
	description={`Discover the current best weather in the United States! This real-time tracker reveals the top outdoor conditions nationwide${unicode.em}perfect for travel planning, road trips, or a spontaneous weekend getaway.`}
/>

<h1 class="text-center">
	The best weather in America is currently in<br />{currentCondition.city ?? '?'}, {currentCondition.state ??
		'?'}.
</h1>

<Hidden
	><Anchor name="current-conditions"><h2 class="text-center">Current Conditions</h2></Anchor
	></Hidden
>

<div class="text-center mt-6">
	The temperature is {currentCondition.temperature ?? '?'} with {currentCondition.humidity ?? '?'} humidity,
	{currentCondition.wind ?? '?'} wind, and {currentCondition.rain ?? '? chance'} of precipitation.
</div>

<iframe bind:this={mapIframe} style="width:100%; height:400px" class="mt-6" title="Map"></iframe>

<div class="text-center mt-6">
	This information was last updated at {currentCondition.easternTimeString ?? '?'} Eastern Time. This
	page updates automatically as new information becomes available.
</div>

<h3 class="mt-6">Table of Contents</h3>

<div class="mt-1">
	1. <Link href="#current-conditions">Current Conditions</Link><br />
	2. <Link href="#what-is-this">What is this?</Link><br />
	3. <Link href="#historical-conditions">Historical Conditions</Link>
</div>

<Anchor name="what-is-this"><h2 class="mt-6">What is this?</h2></Anchor>

<div class="mt-6">
	With most weather apps, you enter a location and the app tells you the weather at that location.
	This app does the opposite: it tells you a location where the weather is good.
</div>

<div class="mt-6">
	This app considers the best outdoor weather to be the following:<br />
	- 70{unicode.degree}F<br />
	- 50% relative humidity<br />
	- 5 mph wind<br />
	- 0% chance of precipitation
</div>

<div class="mt-6">
	Deciding what "best" means in terms of weather is actually quite difficult. Weather preferences
	vary from person to person, but there are some consistent patterns. 20{unicode.degree}C (68{unicode.degree}F)
	is used as the standard for "room temperature". This is the temperature at which the greatest
	number of people feel comfortable indoors.
</div>

<div class="mt-6">
	Outdoor temperature is slightly more complicated due to factors such as sunlight and wind. People
	like having a slight breeze when outdoors; stagnant air outdoors makes people feel uncomfortable.
	Accounting for this, this project picks an ideal temperature that is slightly higher than room
	temperature to account for wind chill. This allows for outdoor recreation without overheating, but
	doesn't necessitate a sweater or physical movement to stay warm. Preferences for humidity and
	precipitation are more consistent across the population, 50% and 0 respectively.
</div>

<div class="mt-6">
	Behind the scenes, this app queries the <Link href="https://www.weather.gov/"
		>US National Weather Service</Link
	> for information about current weather conditions throughout the United States. To maximize performance,
	this app uses a lower spatial resolution than the National Weather Service. This means that the city
	that actually has the best weather at any given time might be several miles away from the city that
	this app claims to have the best weather.
</div>

<Anchor name="historical-conditions"><h2 class="mt-6">Historical Conditions</h2></Anchor>

<div class="mt-3">
	The following is a list of cities that had the best weather in America at some point in the last
	24 hours. Times are listed in US Eastern Time.
</div>

<div class="flex flex-col gap-6 mt-6">
	{#each validConditions as condition}
		<div>
			<div class="flex flex-wrap gap-3">
				<div>{condition.easternTimeString ?? '?'}</div>
				<div>{condition.city ?? '?'}, {condition.state ?? '?'}</div>
			</div>
			<div class="flex flex-wrap gap-3">
				<div>{condition.temperature ?? `?${unicode.degree}F`}</div>
				<div>{condition.humidity ?? '?'} humidity</div>
				<div>{condition.wind ?? '?'} wind</div>
			</div>
			<div class="flex flex-wrap gap-3">
				<div>{condition.rain ?? '? chance'} of precipitation</div>
			</div>
		</div>
	{/each}
</div>
