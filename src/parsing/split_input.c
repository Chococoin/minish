/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_input.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: siellage <siellage@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 16:42:11 by glugo-mu          #+#    #+#             */
/*   Updated: 2026/01/29 17:51:19 by siellage         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ensure_capacity(t_split_ctx *ctx)
{
	char	**new_args;
	size_t	new_capacity;
	size_t	i;

	if (ctx->count < ctx->capacity)
		return (1);
	new_capacity = ctx->capacity * 2;
	new_args = malloc(sizeof(char *) * (new_capacity + 1));
	if (!new_args)
		return (0);
	i = 0;
	while (i < ctx->count)
	{
		new_args[i] = ctx->tokens[i];
		i++;
	}
	free(ctx->tokens);
	ctx->tokens = new_args;
	ctx->capacity = new_capacity;
	return (1);
}

static int	process_quote(char *input, size_t *i, t_split_ctx *ctx)
{
	char	quote;

	quote = input[(*i)++];
	while (input[*i] && input[*i] != quote)
		(*i)++;
	if (!input[*i])
	{
		ctx->quote_error = quote;
		return (0);
	}
	return (1);
}

static int	add_token(t_split_ctx *ctx, char *input, size_t *idx)
{
	size_t	i;

	if (!ensure_capacity(ctx))
		return (0);
	ctx->tokens[ctx->count++] = &input[*idx];
	i = *idx;
	while (input[i] && input[i] != ' ' && input[i] != '\t')
	{
		if (input[i] == '\'' || input[i] == '\"')
		{
			if (!process_quote(input, &i, ctx))
				return (0);
		}
		if (input[i])
			i++;
	}
	if (input[i])
		input[i++] = '\0';
	*idx = i;
	return (1);
}

static int	fill_tokens(t_split_ctx *ctx, char *input)
{
	size_t	i;

	i = 0;
	while (input[i])
	{
		while (input[i] && (input[i] == ' ' || input[i] == '\t'))
			i++;
		if (!input[i])
			break ;
		if (!add_token(ctx, input, &i))
			return (0);
	}
	return (1);
}

char	**split_input(char *input)
{
	t_split_ctx	ctx;

	if (!input)
		return (NULL);
	ctx.capacity = 8;
	ctx.quote_error = 0;
	ctx.tokens = malloc(sizeof(char *) * (ctx.capacity + 1));
	if (!ctx.tokens)
		return (NULL);
	ctx.count = 0;
	if (!fill_tokens(&ctx, input))
	{
		if (ctx.quote_error)
			printf("minishell: unexpected EOF while looking for "
				"matching `%c'\n", ctx.quote_error);
		free(ctx.tokens);
		return (NULL);
	}
	ctx.tokens[ctx.count] = NULL;
	return (ctx.tokens);
}
