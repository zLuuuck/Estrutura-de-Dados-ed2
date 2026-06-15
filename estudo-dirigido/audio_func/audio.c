/*
 * DIFICULDADE (ajuda de IA): a miniaudio e header-only — toda a implementacao
 * fica em um unico arquivo .h. Para que o linker nao encontre simbolos
 * duplicados, a macro MINIAUDIO_IMPLEMENTATION deve ser definida em EXATAMENTE
 * um arquivo .c antes de incluir o header. Se fosse definida em mais de um
 * .c, o compilador geraria multiplas definicoes do mesmo simbolo e o link
 * falharia. Consultamos a IA para entender essa restricao e decidimos isolar
 * a definicao aqui, em audio.c, que e o unico modulo que inclui miniaudio.h.
 *
 * APRENDIZADO: bibliotecas header-only sao praticas para distribuicao (basta
 * copiar um arquivo), mas exigem esse cuidado na compilacao para evitar
 * duplicacao de codigo objeto.
 *
 * ENCAPSULAMENTO: nenhum outro modulo inclui miniaudio.h diretamente.
 * Toda a API de audio fica exposta apenas pelo audio.h de alto nivel,
 * tornando facil trocar o engine de audio no futuro sem tocar no resto do
 * sistema.
 */
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "audio.h"

#include <stdio.h>
#include <string.h>

/* Estado interno — variaveis globais estaticas (visíveis so neste .c) */

static ma_engine g_engine;  /* motor de audio miniaudio            */
static ma_sound g_sound;    /* som atualmente carregado            */
static int g_engine_ok = 0; /* 1 apos audio_init() bem-sucedido    */
static int g_sound_ok = 0;  /* 1 quando ha um som carregado        */

/* Inicializacao / encerramento */

int audio_init(void)
{
    if (g_engine_ok)
    {
        return 1; /* ja inicializado */
    }

    ma_result r = ma_engine_init(NULL, &g_engine);
    if (r != MA_SUCCESS)
    {
        fprintf(stderr, "Erro: falha ao inicializar engine de audio (%d).\n", r);
        return 0;
    }
    g_engine_ok = 1;
    return 1;
}

void audio_shutdown(void)
{
    if (!g_engine_ok)
    {
        return;
    }
    audio_stop(); /* descarrega som se houver */
    ma_engine_uninit(&g_engine);
    g_engine_ok = 0;
}

/* Carregamento */

int audio_load(const char *caminho)
{
    if (!g_engine_ok)
    {
        return 0;
    }

    /* Descarrega som anterior */
    if (g_sound_ok)
    {
        ma_sound_uninit(&g_sound);
        g_sound_ok = 0;
    }

    ma_result r = ma_sound_init_from_file(&g_engine, caminho, MA_SOUND_FLAG_DECODE, NULL, NULL, &g_sound);
    if (r != MA_SUCCESS)
    {
        fprintf(stderr, "Erro: nao foi possivel carregar '%s' (%d).\n", caminho, r);
        return 0;
    }
    g_sound_ok = 1;
    return 1;
}

/* Controles de reproducao */

void audio_play(void)
{
    if (!g_sound_ok)
    {
        return;
    }
    /* Rebobina ao inicio antes de tocar */
    ma_sound_seek_to_pcm_frame(&g_sound, 0);
    ma_sound_start(&g_sound);
}

void audio_pause(void)
{
    if (!g_sound_ok)
    {
        return;
    }
    ma_sound_stop(&g_sound); /* miniaudio: stop = pause (posicao preservada) */
}

void audio_resume(void)
{
    if (!g_sound_ok)
    {
        return;
    }
    ma_sound_start(&g_sound);
}

void audio_seek_relative(float segundos)
{
    if (!g_sound_ok)
    {
        return;
    }

    ma_uint32 sample_rate = ma_engine_get_sample_rate(&g_engine);
    if (sample_rate == 0)
    {
        return;
    }

    /* Posicao atual em frames PCM */
    ma_uint64 pos_atual = 0;
    ma_sound_get_cursor_in_pcm_frames(&g_sound, &pos_atual);

    /* Duracao total em frames */
    ma_uint64 duracao_total = 0;
    ma_sound_get_length_in_pcm_frames(&g_sound, &duracao_total);

    /* Calcula nova posicao com clamping */
    ma_int64 delta = (ma_int64)(segundos * (float)sample_rate);
    ma_int64 nova = (ma_int64)pos_atual + delta;
    if (nova < 0)
    {
        nova = 0;
    }
    if ((ma_uint64)nova > duracao_total)
    {
        nova = (ma_int64)duracao_total;
    }

    ma_sound_seek_to_pcm_frame(&g_sound, (ma_uint64)nova);
}

/* Consultas de estado */

float audio_get_position_seconds(void)
{
    if (!g_sound_ok)
    {
        return 0.0f;
    }
    float pos = 0.0f;
    ma_sound_get_cursor_in_seconds(&g_sound, &pos);
    return pos;
}

float audio_get_duration_seconds(void)
{
    if (!g_sound_ok)
    {
        return 0.0f;
    }
    float dur = 0.0f;
    ma_sound_get_length_in_seconds(&g_sound, &dur);
    return dur;
}

int audio_is_playing(void)
{
    if (!g_sound_ok)
    {
        return 0;
    }
    return ma_sound_is_playing(&g_sound);
}

int audio_is_at_end(void)
{
    if (!g_sound_ok)
    {
        return 0;
    }
    return ma_sound_at_end(&g_sound);
}

/* Stop total */

void audio_stop(void)
{
    if (!g_sound_ok)
    {
        return;
    }
    ma_sound_stop(&g_sound);
    ma_sound_uninit(&g_sound);
    g_sound_ok = 0;
}
